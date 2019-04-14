#include <Arduino.h>
#include "fifo.h"
#include "display.h"
#include <GyverButton.h>

#define RESET 6
#define START 7

#define BEEP 9

#define ALL_KEYS  0b00111100
#define TEAM_KEYS 0b00111100

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};

Display display;

FIFO keysFIFO;
uint32_t lastReset;

State state;
uint32_t start;
uint8_t left_time;
uint32_t timeStart;

volatile uint8_t enabled = ALL_KEYS;
uint8_t disabled = ~ALL_KEYS;

GButton reset_btn(RESET);
GButton start_btn(START);

// void pciSetup(uint8_t pin)
// {
//     *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
//     PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
//     PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
// }

void setup(void)
{
    reset_btn.setTickMode(true);
    start_btn.setTickMode(true);
    //set D2-7
    DDRD  &= ~ALL_KEYS;
    PORTD |= ALL_KEYS;
    // pinMode(A0, INPUT_PULLUP);
    // pinMode(A1, INPUT_PULLUP);
    // pinMode(A2, INPUT_PULLUP);
    // pinMode(A3, INPUT_PULLUP);
    // pinMode(RESET, INPUT_PULLUP);
    // pinMode(START, INPUT_PULLUP);

    *digitalPinToPCMSK(START) |= ALL_KEYS;  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(START)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(START)); // enable interrupt for the group

    pinMode(BEEP, OUTPUT);
    pinMode(13, OUTPUT);

    display.begin();
    Serial.begin(9600);
}


void lastTone(void){
    tone(BEEP, 500, 100);
}

void timeOutTone(void){
    tone(BEEP, 600, 1000);
}

void falseTone(void){
    tone(BEEP, 700, 1000);
}

void teamTone(void){
    tone(BEEP, 800, 1000);
}

void readTeams(FIFORow &key){
    uint8_t mask = 0b100;
    for(uint8_t i = 0; i < 4; i++) {
        if (key.pins & mask & ~disabled) {
            if (state == state_read) {
                state = state_false;
                falseTone();
                display.printTeamFs(i);
            }
            if (state == state_time) {
                state = state_answer;
                teamTone();
                display.printTeam(i, key.time - timeStart);
            }
            
            disabled |= mask;
        }
        mask <<= 1;
    }
}

void readHost() {
    if (reset_btn.isPress()) {
        if (state != state_false) {
            enabled = ALL_KEYS;
            disabled = ~ALL_KEYS;
        }
        state = state_read;
        left_time = 0;
        u8g2.clear();
        
        Serial.println("reset");
    }
    if (state != state_read && state != state_answer) return;
    if (start_btn.isPress()) {
        left_time = state == state_read ? 61 : 21;
        state = state_time;
        timeStart = millis();
        start = timeStart - 1000;

        Serial.println("start");
    }
}

void readKeys(void){
    while(!keysFIFO.isEmpty()) {
        auto key = keysFIFO.get();
        if (state == state_read || state == state_time) {
            readTeams(key);
        }
        //readHost(key.pins);
    }
    
    enabled = ~disabled;
}

void updateTimer(void) {
    if (state != state_time){
        return;
    }
    auto cur = millis();
    if (cur - start >= 1000){
        start += 1000;
        left_time--;
        
        display.printTime(left_time);

        if (left_time <= 0){
            timeOutTone();
            state = state_end;
            return;
        }
        if (left_time <= 3){
            lastTone();
            return;
        }
    }
}



void loop(void)
{
    // auto ms = millis();
    readKeys();
    updateTimer();
    readHost();
    // ms = millis() - ms;
    // if (ms > 2) Serial.println(ms);
}

uint8_t prev_pins = 0;
// handle pin change interrupt for D0 to D7 here
ISR (PCINT2_vect)
{
    uint8_t new_pins = ~PIND;

    uint8_t pins = (new_pins ^ prev_pins) & new_pins & enabled;
    prev_pins = new_pins;

    if (!pins) return;
    enabled &= ~pins;

    keysFIFO.add(pins);
}
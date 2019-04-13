#include <Arduino.h>
#include "team.h"
#include "display.h"

#define RESET 6
#define START 7

#define BEEP 9

#define ALL_KEYS  0b11111100
#define TEAM_KEYS 0b00111100

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};

const char fal[] = " FS";

Display display;

FIFO keysFIFO;
uint32_t lastReset;

State state;
uint32_t start;
uint8_t left_time;
uint32_t timeStart;

volatile uint8_t enabled = ALL_KEYS;
uint8_t disabled = ~ALL_KEYS;

// void pciSetup(uint8_t pin)
// {
//     *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
//     PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
//     PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
// }

void setup(void)
{
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
    }
}

void readHost(uint8_t pins){
    // if (millis() - lastReset < 1000) {
    //     lastReset = millis();
    //     return;
    // }
    // lastReset = millis();

    Serial.println(pins, 2);
    Serial.println(enabled, 2);
    Serial.println(disabled, 2);
    Serial.print(keysFIFO.inId);
    Serial.print(' ');
    Serial.println(keysFIFO.outId);

    if (pins & bit(digitalPinToPCMSKbit(RESET))) {
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
    if (pins & bit(digitalPinToPCMSKbit(START))) {
        lastReset = millis();
        left_time = state == state_read ? 61 : 21;
        state = state_time;
        timeStart = millis();
        start = timeStart - 1000;

        Serial.println("start");
    }
}

void readKeys(void){
    while(!keysFIFO.isEmpty()){
        auto key = keysFIFO.get();
        if (state == state_read || state == state_time) {
            readTeams(key);
        }
        readHost(key.pins);
    }
    enabled = ~disabled;
}

void updateTimer(void){
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
    // ms = millis() - ms;
    // if (ms > 2) Serial.println(ms);
    //delay(1000);
}

// handle pin change interrupt for D0 to D7 here
ISR (PCINT2_vect)
{
    auto pins = (~PIND) & enabled;
    if (!pins) return;
    enabled &= ~pins;

    keysFIFO.add(pins);
}

#include <Arduino.h>
#include "TM1637.h"

#define RESET 6
#define START 7
#define CLK 8
#define DIO 9
#define BEEP 10

#define ALL_KEYS 0b11111100
#define TEAM_KEYS 0b00111100

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};

TM1637 display(CLK,DIO);
//TM1637Display display(CLK, DIO);
State state;

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

    pinMode(BEEP, OUTPUT);

    Serial.begin(9600);
}

uint8_t buttons;
uint8_t enabled = TEAM_KEYS;

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

uint8_t bitToTeam(void){
    uint8_t mask = 0b100;
    for(uint8_t i = 1; i <= 4; i++)
    {
        if (buttons & mask) {
            enabled &= ~mask;
            return i;
        }
        mask <<= 1;
    }
    return 0;
}

void displayTeam(uint8_t team){
    display.display(1, team);
}

void readMainPins(void){
    buttons = PINC & enabled;
    if (!buttons) return;

    if (state == state_read){
        state = state_false;

        auto team = bitToTeam();
        displayTeam(team);
        falseTone();
        return;
    }
    if (state == state_time){
        state = state_answer;

        auto team = bitToTeam();
        displayTeam(team);
        teamTone();
        return;
    }
}

unsigned long start;
uint8_t left_time;

void updateTimer(void){
    if (state != state_time){
        return;
    }
    auto cur = millis();
    if (cur - start >= 1000){
        start += 1000;
        left_time--;
        
        display.display(2, left_time / 10);//??
        display.display(3, left_time % 10);//??

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

void readKeys(void){
    if (digitalRead(RESET)) {
        if (state != state_false) enabled = TEAM_KEYS;
        state = state_read;
        left_time = 0;
        display.clearDisplay();
    }
    if (state != state_read && state != state_answer) return;
    if (digitalRead(START)) {
        state = state_time;
        start = micros() - 1000000;
        left_time = state == state_read ? 61 : 21;
    }
}

void loop(void)
{
    readMainPins();
    readKeys();
    updateTimer();
}

// handle pin change interrupt for D0 to D7 here
// ISR (PCINT2_vect)
// {
//     digitalWrite(13,digitalRead(7));
// }
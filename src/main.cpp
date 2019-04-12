
#include <Arduino.h>
//#include <TimerOne.h>
//#include "TM1637.h"
#include <TM1637Display.h>

#define RESET 2
#define START 3
#define BEEP 6
#define DIO 7
#define CLK 8
#define TEAM_KEYS 0b1111

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};

//TM1637 tm1637(CLK,DIO);
TM1637Display display(CLK, DIO);
State state;

void setup(void)
{
    //set A0-3
    DDRC  &= ~TEAM_KEYS;
    PORTC |= TEAM_KEYS;
    // pinMode(A0, INPUT_PULLUP);
    // pinMode(A1, INPUT_PULLUP);
    // pinMode(A2, INPUT_PULLUP);
    // pinMode(A3, INPUT_PULLUP);

    pinMode(RESET, INPUT_PULLUP);
    pinMode(START, INPUT_PULLUP);

    pinMode(BEEP, OUTPUT);
    pinMode(DIO, OUTPUT);
    pinMode(CLK, OUTPUT);

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
    uint8_t mask = 1;
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
    display.showNumberDec(team, false, 1, 1);//??
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
    auto cur = micros();
    if (cur - start >= 1000000){
        start = cur;
        left_time--;
        
        display.showNumberDec(left_time, true, 2, 2);//??

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
        state = state_read;
        left_time = 0;
        display.clear();
        enabled = TEAM_KEYS;
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

ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
    digitalWrite(13,digitalRead(7));
}
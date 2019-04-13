
#include <Arduino.h>
#include <U8g2lib.h>
//#include <Wire.h>
#include "team.h"

#define RESET 6
#define START 7

#define BEEP 10

#define ALL_KEYS 0b11111100
#define TEAM_KEYS 0b00111100

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};

const char fal[] = " FS";

State state;
unsigned long start;
uint8_t left_time;
Team teams[4];


uint8_t disabled = ~TEAM_KEYS;
volatile uint8_t enabled = TEAM_KEYS;
volatile unsigned long timeStart;

void pciSetup(uint8_t pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
void setup(void)
{
    for(uint8_t i = 0; i < 4; i++)
        teams[i].clear();

    //set D2-7
    DDRD  &= ~ALL_KEYS;
    PORTD |= ALL_KEYS;
    // pinMode(A0, INPUT_PULLUP);
    // pinMode(A1, INPUT_PULLUP);
    // pinMode(A2, INPUT_PULLUP);
    // pinMode(A3, INPUT_PULLUP);
    // pinMode(RESET, INPUT_PULLUP);
    // pinMode(START, INPUT_PULLUP);

    for(uint8_t i = 2; i < 2+4; i++)
        pciSetup(i);

    pinMode(BEEP, OUTPUT);
    pinMode(13, OUTPUT);

    u8g2.begin();
    Serial.begin(9600);
    Serial.println(fal);
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

void displayTeam(uint8_t id){
    static char m_str[3+4];
    strcpy(m_str, u8x8_u8toa(id+1, 2));
    if (state != state_false){
        m_str[2] = ' ';
        uint16_t ms = (teams[id].time - start) % 1000;
        strcpy(m_str+3, u8x8_u16toa(ms, 3));
    }
    else{
        strcpy(m_str+2, fal);
    }

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g_font_9x15B);
        u8g2.drawStr(0,32,m_str);
        u8g2.drawStr(0,14,m_str);
    } while ( u8g2.nextPage() );
}

void updateMainPins(void){

    uint8_t mask = 0b100;
    for(uint8_t i = 0; i < 4; i++) {
        if (teams[i].isFire) {
            teams[i].isFire = false;
            if (state != state_read && state != state_time) continue;

            if (state == state_read) {
                state = state_false;
                falseTone();
            }
            if (state == state_time) {
                state = state_answer;
                teamTone();
            }
            
            displayTeam(i);
            disabled |= mask;
        }
        mask <<= 1;
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
        
        static char m_str[3];
        strcpy(m_str, u8x8_u8toa(left_time, 2));
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_logisoso32_tn);
            u8g2.drawStr(64,32,m_str);
        } while ( u8g2.nextPage() );

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
    if (!digitalRead(RESET)) {
        auto ms = millis();
        while((millis() - ms) < 100) {
            if (!digitalRead(RESET)) ms = millis();
            yield();
        }

        if (state != state_false) {
            enabled = TEAM_KEYS;
            disabled = ~TEAM_KEYS;
        }
        state = state_read;
        left_time = 0;
        u8g2.clear();
        // u8g2.firstPage();
        // do {
        //     // u8g2.drawLine(0,0,32,32);
        //     // u8g2.drawLine(32,0,0,32);
        // } while ( u8g2.nextPage() );
    }
    if (state != state_read && state != state_answer) return;
    if (!digitalRead(START)) {
        auto ms = millis();
        while((millis() - ms) < 100) {
            if (!digitalRead(START)) ms = millis();
            yield();
        }

        left_time = state == state_read ? 61 : 21;
        state = state_time;
        timeStart = millis();
        start = timeStart - 1000;

//        Serial.println("start");
    }
}

void loop(void)
{
    // auto ms = millis();
    updateMainPins();
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

    uint8_t mask = 0b100;
    for(uint8_t i = 0; i < 4; i++)
    {
        if (pins & mask) {
            teams[i].time = millis() - timeStart;
            teams[i].isFire = true;
        }
        mask <<= 1;
    }
}
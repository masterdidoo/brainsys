
#include <Arduino.h>
#include <U8g2lib.h>
//#include <Wire.h>

#define RESET 6
#define START 7

#define BEEP 10

#define ALL_KEYS 0b11111100
#define TEAM_KEYS 0b00111100

enum State
{
    state_read, state_time, state_answer, state_false, state_end
};


State state;
unsigned long start;
uint8_t left_time;

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
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

    for(auto i=2;i<=7;i++){
        pciSetup(i);
    }

    pinMode(BEEP, OUTPUT);
    pinMode(13, OUTPUT);

    u8g2.begin();
    //Serial.begin(9600);
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
    static char m_str[3+4];
    strcpy(m_str, u8x8_u8toa(team, 2));
    if (state != state_false){
        m_str[2] = ' ';
        auto cur = millis();
        uint16_t ms = cur - start;
        strcpy(m_str+3, u8x8_u16toa(ms, 3));
    }
    else{
        strcpy(m_str+2, " fal");
    }

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_logisoso32_tn);
        u8g2.drawStr(0,32,m_str);
    } while ( u8g2.nextPage() );
}

void readMainPins(void){
    buttons = (~PIND) & enabled;
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
        if (state != state_false) {
            enabled = TEAM_KEYS;
        }
        state = state_read;
        left_time = 0;
        u8g2.clear();
        auto ms = millis();
        while((millis() - ms) < 100) {
            if (!digitalRead(RESET)) ms = millis();
            yield();
        }
        // u8g2.firstPage();
        // do {
        //     // u8g2.drawLine(0,0,32,32);
        //     // u8g2.drawLine(32,0,0,32);
        // } while ( u8g2.nextPage() );
    }
    if (state != state_read && state != state_answer) return;
    if (!digitalRead(START)) {
        left_time = state == state_read ? 61 : 21;
        state = state_time;
        start = millis() - 1000;

        auto ms = millis();
        while((millis() - ms) < 100) {
            if (!digitalRead(RESET)) ms = millis();
            yield();
        }
//        Serial.println("start");
    }
}

void loop(void)
{
    // auto ms = millis();
    readMainPins();
    readKeys();
    updateTimer();
    // ms = millis() - ms;
    // if (ms > 2) Serial.println(ms);
    //delay(1000);
}

auto x = HIGH;

// handle pin change interrupt for D0 to D7 here
ISR (PCINT2_vect)
{
    x = !x;
    digitalWrite(13, x);
}
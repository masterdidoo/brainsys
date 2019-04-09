
#include <Arduino.h>
//#include <TimerOne.h>
//#include "TM1637.h"
#include <TM1637Display.h>

#define DIO 7
#define CLK 8
#define START 7
#define STOP 8
#define TEAM_KEYS 0b1111

enum state
{
    state_read, state_time, state_answer
};

//TM1637 tm1637(CLK,DIO);
TM1637Display display(CLK, DIO);

void setup(void)
{
    //set A0-3
    DDRC  &= ~TEAM_KEYS;
    PORTC |= TEAM_KEYS;
    // pinMode(A0, INPUT_PULLUP);
    // pinMode(A1, INPUT_PULLUP);
    // pinMode(A2, INPUT_PULLUP);
    // pinMode(A3, INPUT_PULLUP);

    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(12, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(10, INPUT);
    pinMode(PIND7 , INPUT);

//    Timer1.attachInterrupt(read);
}

uint8_t buttons;

void readMainPins(void){
    buttons = PINC & TEAM_KEYS;
}

void loop(void)
{
    readMainPins();

    auto start = micros();
    auto data = digitalRead(A0);
//    auto data = analogRead(A0);
    auto dur = micros() - start;
    Serial.print(F("analogRead="));
    Serial.println(dur);
    Serial.print(F("data="));
    Serial.println(data);

    start = micros();
    data = PINB;
    auto pin12 = data & (1 << 4);
    auto pin11 = data & (1 << 3);
    auto pin10 = data & (1 << 2);
    auto pin9 = data & (1 << 5);

    dur = micros() - start;
    Serial.print(F("digitalRead="));
    Serial.println(dur);
    Serial.print(F("data="));
    Serial.println(data, 2);
    
    Serial.print(digitalRead(12));
    Serial.print(' ');
    Serial.println(pin12);

    Serial.println(pin11);
    Serial.println(pin10);
    Serial.println(pin9);

//-----

//    PORTB ^= 1 << 5;
//    delay(100);
//    PORTB ^= 1 << 5;
    delay(1000);
}
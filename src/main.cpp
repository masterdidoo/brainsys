#include <Arduino.h>

void setup(void)
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(12, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(10, INPUT);
    pinMode(9, INPUT);
}

void loop(void)
{
    auto start = micros();
    auto data = analogRead(A0);
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
#include <Arduino.h>

uint8_t inline digitalRead2(uint8_t pin)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	if (*portInputRegister(port) & bit) return HIGH;
	return LOW;
}

void setup(void)
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(12, INPUT);
    pinMode(11, INPUT);
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
    data = PORTB;
    auto pin12 = data & (1 << 4);
    auto pin11 = data & (1 << 3);
    auto pin10 = data & (1 << 2);
    auto pin9 = data & (1 << 5);

    dur = micros() - start;
    Serial.print(F("digitalRead="));
    Serial.println(dur);
    Serial.print(F("data="));
    Serial.println(data, 2);
    Serial.println(pin12);
    Serial.println(pin11);
    Serial.println(pin10);
    Serial.println(pin9);

    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

//-----

    PORTB ^= 1 << 5;
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
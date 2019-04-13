#ifndef time_h
#define time_h

#include <Arduino.h>

struct Team
{
    bool isFire;
    unsigned long time;

    void clear(void) {
        isFire = false;
        time = 0;
    }
};

struct FIFORow
{
    uint8_t pins;
    uint32_t time;
};

struct FIFO
{
    uint8_t inId  = 0;
    uint8_t outId = 0;

    FIFORow buff[0xf+1];

    inline bool isEmpty(void) {
        return inId == outId;
    }

    inline FIFORow& get(void) {
        auto i = outId;
        ++outId &= 0xf;
        return buff[i];
    }

    inline void add(uint8_t pins) {
        buff[inId].pins = pins;
        buff[inId].time = millis();
        ++inId &= 0xf;
    }
};

#endif
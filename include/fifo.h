#ifndef fifo_h
#define fifo_h

#include <Arduino.h>

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

    inline void clear(void) {
        outId = inId;
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
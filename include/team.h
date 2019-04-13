#ifndef time_h
#define time_h

struct Team
{
    bool isFire;
    unsigned long time;

    void clear(void) {
        isFire = false;
        time = 0;
    }
};

#endif
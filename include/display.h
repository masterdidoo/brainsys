#ifndef display_h
#define display_h

#include <U8g2lib.h>
//#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

struct Display {
    void begin(void) {
        u8g2.begin();
    }

    void printTeamFs(uint8_t id){
        char m_str[] = "1 FS";
        m_str[0] += id;

        u8g2.firstPage();
        do {
            u8g2.setFont(u8g_font_9x15B);
            u8g2.drawStr(0,14,m_str);
        } while ( u8g2.nextPage() );
    }

    void printTeam(uint8_t id, uint16_t time){
        char m_str[] = "1 xxxxx";
        m_str[0] += id;
        strcpy(m_str+2, u8x8_u16toa(time, 5));

        u8g2.firstPage();
        do {
            u8g2.setFont(u8g_font_9x15B);
            u8g2.drawStr(0,14,m_str);
        } while ( u8g2.nextPage() );
    }

    void printTime(uint8_t time){
        char m_str[3];
        strcpy(m_str, u8x8_u8toa(time, 2));
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_logisoso32_tn);
            u8g2.drawStr(64,32,m_str);
        } while ( u8g2.nextPage() );
    }
};

#endif
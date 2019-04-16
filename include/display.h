#ifndef display_h
#define display_h

#include <U8g2lib.h>
//#include <Wire.h>

#define FONTx1 u8x8_font_5x8_n
#define FONTx2 u8x8_font_7x14B_1x2_r
#define FONTx4 u8x8_font_7x14B_1x2_r
//#define FONTx4 u8x8_font_inb21_2x4_r

//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 

struct Display {
    void begin(void) {
        u8x8.begin();
        clear(0);
    }

    void printTeamFs(uint8_t id){
        digitalWrite(A0+id, HIGH);
        char m_str[] = "1 FS";
        m_str[0] += id;

        u8x8.setFont(FONTx4);
        u8x8.draw2x2String(2,0,m_str);
    }

    void inline printTeamInt(uint8_t id, uint16_t time, u8g2_uint_t row){
        char m_str[] = "1 xxxxx";
        m_str[0] += id;
        strcpy(m_str+2, u8x8_u16toa(time, 5));

        u8x8.setFont(FONTx2);
        u8x8.drawString(2,row,m_str);
    }

    void printTeam(uint8_t id, uint16_t time){
        digitalWrite(A0+id, HIGH);
        printTeamInt(id, time, 0);
    }

    void printTeam2(uint8_t id, uint16_t time){
        printTeamInt(id, time, 2);
    }

    void printTime(uint8_t time){
        char m_str[3];
        strcpy(m_str, u8x8_u8toa(time, 2));

        u8x8.setFont(FONTx4);
        u8x8.draw2x2String(10,0,m_str);
    }

    void clear(uint8_t bits) {
        u8x8.clear();
        u8x8.setFont(FONTx1);
        for (int i=0;i<4;i++){
            digitalWrite(A0+i, LOW);
            if (bits & bit(i+2)) continue;
            u8x8.setCursor(0,i);
            u8x8.print(i+1);
        }
    }
};

#endif
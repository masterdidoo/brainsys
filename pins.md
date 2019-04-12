* d2 int0                       reset           t1
* d3 int1 PWM+2                 start           t2
* d4                 CSN NRF24                  t3
* d5      PWM+0      CE  NRF24                  t4
* d6      PWM+0                 beeper          reset
* d7                            display dio     start
* d8                            display clk     display clk
* d9      PWM-1                                 display dio
* d10     PWM-1                                 beep
* d11     PWM-2 MOSI
* d12           MISO
* d13 led       SCK

* a0                            t1              t1d
* a1                            t2              t2d
* a2                            t3              t3d
* a3                            t4              t4d
* a4            SDA
* a5            SCL
* a6 a.in & rst
* a7 a.in

* timer0                        
* timer1                        cloc
* timer2                        beeper
#ifndef ttt_serial_h
#define ttt_serial_h

#include <arduino.h>

//#define OUTPUT_UART   // when this is defined, all output will be directed to the standard UART serial port
                      // when not defined, output is directed to a software serial port (when PIXY attached)

#define UART_BAUD   9600
#define SOFTW_BAUD  9600
#define TX_PIN      9
#define RX_PIN      8

class TTT_Serial {
  public:
    TTT_Serial();
    void begin();
    void print(char *val);
    void print(int val);
    void print(float val);
    void print(double val);
    void println(char *val);
    void println(int val);
    void println(float val);
    void println(double val);
    void print(const __FlashStringHelper* val);
    void println(const __FlashStringHelper* val);

    byte available();
    byte read();

};

extern TTT_Serial ttt_serial;
#endif


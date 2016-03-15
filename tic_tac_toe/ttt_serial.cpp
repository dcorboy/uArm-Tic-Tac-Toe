#include "ttt_serial.h"

#ifndef OUTPUT_UART
#include <SoftwareSerial.h>
SoftwareSerial proxySerial(RX_PIN, TX_PIN);  // Rx, Tx
#endif

TTT_Serial ttt_serial;

TTT_Serial::TTT_Serial() {}

#ifdef OUTPUT_UART

void TTT_Serial::begin() { Serial.begin(UART_BAUD); } // start standard serial port at desired bps
void TTT_Serial::print(char *val) { Serial.print(val); }
void TTT_Serial::print(int val) { Serial.print(val); }
void TTT_Serial::print(float val) { Serial.print(val); }
void TTT_Serial::print(double val) { Serial.print(val); }
void TTT_Serial::println(char *val) { Serial.println(val); }
void TTT_Serial::println(int val) { Serial.println(val); }
void TTT_Serial::println(float val) { Serial.println(val); }
void TTT_Serial::println(double val) { Serial.println(val); }
void TTT_Serial::print(const __FlashStringHelper* val) { Serial.print(val); }
void TTT_Serial::println(const __FlashStringHelper* val) { Serial.println(val); }
byte TTT_Serial::available() { return Serial.available(); }
byte TTT_Serial::read() { return Serial.read(); }

#else

void TTT_Serial::begin() { proxySerial.begin(SOFTW_BAUD); } // start software serial port at desired bps
void TTT_Serial::print(char *val) { proxySerial.print(val); }
void TTT_Serial::print(int val) { proxySerial.print(val); }
void TTT_Serial::print(float val) { proxySerial.print(val); }
void TTT_Serial::print(double val) { proxySerial.print(val); }
void TTT_Serial::println(char *val) { proxySerial.println(val); }
void TTT_Serial::println(int val) { proxySerial.println(val); }
void TTT_Serial::println(float val) { proxySerial.println(val); }
void TTT_Serial::println(double val) { proxySerial.println(val); }
void TTT_Serial::print(const __FlashStringHelper* val) { proxySerial.print(val); }
void TTT_Serial::println(const __FlashStringHelper* val) { proxySerial.println(val); }
byte TTT_Serial::available() { return proxySerial.available(); }
byte TTT_Serial::read() { return proxySerial.read(); }

#endif


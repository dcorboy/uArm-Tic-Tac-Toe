// Test the proxying of data over a software serial connection
// This should be run on a spare Arduino, NOT the uARM (which uses pin 13 for a servo)
// to test the ability to proxy a serial connection to the USB UART
// LIKE the uARM, it is set up to use uArm-available pins D8 and D9

#include <SoftwareSerial.h>

#define LED 13

SoftwareSerial proxySerial(9, 8);  // Rx, Tx
bool feefi = true;

void setup() {
  Serial.begin(9600);
  proxySerial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (proxySerial.available() > 0) {
    byte input = proxySerial.read();
    if (input == 't') {
      feefi = !feefi;
    }
  }
  if (feefi) {
    proxySerial.println("Fee fi..");
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);              // wait for a second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(500);
  } else {
    proxySerial.println("fo fum!");
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);              // wait for a second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(250);              // wait for a second
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);              // wait for a second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(250);              // wait for a second
    delay(500);
  }
}

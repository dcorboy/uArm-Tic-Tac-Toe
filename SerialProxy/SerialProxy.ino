// Exchange data between a software serial port and the USB UART
// Used to provide USB console access for debugging/whatever over any 2 available pins of a remove microcontroller

#include <SoftwareSerial.h>

SoftwareSerial proxySerial(2, 3);  // Rx, Tx

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  proxySerial.begin(9600);
  Serial.println(F("Beginning proxy now..."));
}

void loop() {
  while (proxySerial.available() > 0) {
    Serial.write(proxySerial.read());
  }
  while (Serial.available() > 0) {
    proxySerial.write(Serial.read());
    //Serial.println(F("Sent!"));
  }
}

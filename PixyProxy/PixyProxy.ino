// Test the proxying of PIXY data over a software serial connection
// This version is meant to be run on a uArm with a PIXY set up for UART communication
// and connected to the uArm UART header

// using the included SerialProxy sketch on another Arduino, connect pin D8 on the uArm to pin D2 on the proxy Arduino
// and pind D9 on the uArm to pin D3 on the proxy Arduino
// See Pixy block info via the proxy Arduino's serial port

#include <SoftwareSerial.h>
//#include "myPixyUART.h"

SoftwareSerial proxySerial(9, 8);  // Rx, Tx
//PixyUART pixy;

void setup()
{
  proxySerial.begin(9600); // 9600 baud for the proxy *console* (not for the UART connected to Pixy)
  proxySerial.print("Starting...\n");
  //pixy.init();
}

void loop() {
#if 1
  proxySerial.print("Test\n");
  delay(1000);
#else
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  char pbuf[128]; 
  
  blocks = pixy.getBlocks();
  
  if (blocks)
  {
    i++;
    
   // do this (print) every 50 frames because printing every
   // frame would bog down the Arduino
   if (i%50==0)
    {
      sprintf(buf, "Detected %d:\n", blocks);
      proxySerial.print(buf);
      for (j=0; j<blocks; j++)
      {
        sprintf(buf, "  block %d: ", j);
        proxySerial.print(buf); 
        pixy.blocks[j].print(pbuf);
        proxySerial.print(pbuf);
      }
    }
  }
#endif
}

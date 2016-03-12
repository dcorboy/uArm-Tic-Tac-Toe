// Test the proxying of PIXY data over a software serial connection
// This has only been tested on a spare Arduino, NOT the uARM/
// Works with another Arduino running SerialProxy
// LIKE the uARM, it is set up to use uArm-available pins D8 and D9

#include <SoftwareSerial.h>
#include "myPixyUART.h"

SoftwareSerial proxySerial(9, 8);  // Rx, Tx
PixyUART pixy;

void setup()
{
  proxySerial.begin(9600); // 9600 baud for the proxy *console* (not for the UART connected to Pixy)
  proxySerial.print("Starting...\n");
  pixy.init();
}

void loop()
{
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
}

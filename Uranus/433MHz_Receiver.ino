/*
 Copyright (c) 2013 indrajit Chakrabarty (indyfromoz@gmail.com)

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
 the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/* 

 This sketch a 433MHz receiver to receive messages from remote bots. The VirtualWire library is used for 
 the setup and message reception with the 433MHz RX module . 
 
 VirtualWire library - http://www.open.com.au/mikem/arduino/VirtualWire/index.html

 The PIN connections in the sketch are as - 

 433MHz Rx - 
           TX  = Digital Pin # 11
           VCC = 5V
           GND = ground rail
*/

/* 

  Current version: 0.1
  Version 0.1 - 433MHz Rx

*/ 

#include "VirtualWire.h"


const int ledPin = 13;
char MessageBuffer[VW_MAX_MESSAGE_LEN]; 

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    vw_setup(2000); // Receive at 2000 bits/second
    vw_rx_start();
}

void loop() {
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    
    // Non-blocking
    if (vw_get_message(buf, &buflen)) 
    {
	int i;

        // Turn on a light to show received good message 
        digitalWrite(ledPin, true); 
	
        // Message with a good checksum received, dump it. 
        for (i = 0; i < buflen; i++)
	    {            
          // Fill Sensor1CharMsg Char array with corresponding 
          // chars from buffer.   
          MessageBuffer[i] = char(buf[i]);
	    }
        
        // Null terminate the char array
        // This needs to be done otherwise problems will occur
        // when the incoming messages has less digits than the
        // one before. 
        MessageBuffer[buflen] = '\0';
                
        Serial.print("Data received: ");
        Serial.println(MessageBuffer);
                        
        // Turn off light to and await next message 
        digitalWrite(ledPin, false);
    }
}

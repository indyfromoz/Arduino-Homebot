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
 the setup and message reception with the 433MHz RX module. The Freetronic Ethernet shield (based on W5100)
 is used to upload data to COSM.
 
 VirtualWire library - http://www.open.com.au/mikem/arduino/VirtualWire/index.html
 Freetronics Ethernet shield - http://www.freetronics.com/collections/ethernet/products/ethernet-shield-with-poe

 The PIN connections in the sketch are as - 

 433MHz Rx - 
           TX  = Digital Pin # 11
           VCC = 5V
           GND = ground rail

 Freetronic Ethernet Shield - 
           Pins 10, 11, 12, 13 are used for SPI
*/

/* 

  Current version: 0.2
  Version 0.1 - 433MHz Rx
  Version 0.2 - 433MHz Rx with COSM integration

  NOTE: To use this version, you must use version 0.2 or above of the transmitter sketch

*/ 


#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>
#include "VirtualWire.h"

#include "APIKeys.h"

const int ledPIN = 13;
char MessageBuffer[VW_MAX_MESSAGE_LEN]; 

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned long lastConnectionTime = 0;                // last time we connected to Cosm
const unsigned long connectionInterval = 15000;      // delay between connecting to Cosm in milliseconds

// Define the string for our datastream ID
String pressureSensorId("Pressure");
String humiditySensorId("Humidity");
String temperatureSensorId("Temperature");

CosmDatastream datastreams[] = {
  CosmDatastream(pressureSensorId, DATASTREAM_STRING),
  CosmDatastream(humiditySensorId, DATASTREAM_STRING),
  CosmDatastream(temperatureSensorId, DATASTREAM_STRING),
};

// Wrap the datastream into a feed
CosmFeed feed(FEED_ID, datastreams, 3 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);

void setup() {
  
    // Visual indication of data upload
    pinMode(ledPIN, OUTPUT);
    
    // Keep LED on till we get an IP address
    digitalWrite(ledPIN, true);     
    
    // put your setup code here, to run once:
    Serial.begin(9600);

    Serial.println("Initializing network");
    while (Ethernet.begin(mac) != 1) {
       Serial.println("Error getting IP address via DHCP, trying again...");
       delay(15000);
    }
    
    delay(2000);

    Serial.print("Network initialized with IP Address = ");
    Serial.println(Ethernet.localIP());

    // Turn off LED since we got an IP address
    digitalWrite(ledPIN, false);     
  
    // Initialize VirtualWire
    vw_set_rx_pin(2);
    vw_setup(2000); // Receive at 2000 bits/second
    vw_rx_start(); 
}

/*

  A message is received every 10 seconds. It is either of the format - Ta27.00H51.00Tb26.84P100441
  or Tb26.77P100440

*/

void loop() {

    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    
    // Non-blocking
    if (vw_get_message(buf, &buflen)) 
    {
        int i;

        // Turn on the LED to show received good message 
        digitalWrite(ledPIN, true); 
  
        // Message with a good checksum received, dump it. 
        for (i = 0; i < buflen; i++)
        {            
            // Fill MessageBuffer Char array with corresponding 
            // chars from buffer.   
            MessageBuffer[i] = char(buf[i]);
        }
        
        // Null terminate the char array
        // This needs to be done otherwise problems will occur
        // when the incoming messages has less digits than the
        // one before. 
        MessageBuffer[buflen] = '\0';
        String rawValue(MessageBuffer);
        
        if (rawValue.length() > 0)
        {
            Serial.print("Raw value = ");
            Serial.println(rawValue);
            
            // Extract the individual paramters & values from a string in the format 
            // Ta27.00H51.00Tb26.84P100441 or Tb26.77P100440
            if (rawValue.startsWith("Ta")) // We have temperature aux and humidity
            {
                int humIndex = rawValue.indexOf("H");
                if (humIndex > 0)
                {
                    String temperatureAux = rawValue.substring(2, humIndex);

                    // Check for Tb  
                    int tbIndex = rawValue.indexOf("Tb");
                    if (tbIndex > 0)
                    {
                        String humidity = rawValue.substring(humIndex + 1, tbIndex);
                        
                        int prsIndex = rawValue.indexOf("P");
                        if (prsIndex > 0)
                        {
                            String temperatureMain = rawValue.substring(tbIndex + 2, prsIndex);
                            String pressure = rawValue.substring(prsIndex + 1, rawValue.length());
                            
                            datastreams[0].setString(pressure);
                            datastreams[1].setString(humidity);
                            datastreams[2].setString(temperatureMain);
                            //datastreams[3].setString(temperatureAux);
                        }
                    }
                }
            }
            else if (rawValue.startsWith("Tb")) // We have temperature main and pressure
            {
                int prsIndex = rawValue.indexOf("P");
                String temperatureMain = rawValue.substring(2, prsIndex);
                String pressure = rawValue.substring(prsIndex + 1, rawValue.length());

                String humidity("0");
                String temperatureAux("0");
                
                datastreams[0].setString(pressure);
                datastreams[1].setString(humidity);
                //datastreams[2].setString(temperatureMain);
                //datastreams[2].setString(temperatureAux);                
            }  
        
            int status = cosmclient.put(feed, API_KEY);
            Serial.print("Status of upload: ");
            Serial.println(status);            
        }
               
        // Turn off the LED and await next message 
        digitalWrite(ledPIN, false);
    }  
}


    

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

 This sketch uses a BMP085 temperature+pressure sensor and a DHT11 temperature+humidity sensor 
 connected to an Arduino along with a 433MHz transmitter. The Adafruit BMP085 library is used for 
 the temperature, pressure and altitude measurements with the BMP085. The Adafruit DHT11/DHT22 
 library is used for the measurement of temperature and relative humidity. The VirtualWire library 
 is used for the setup and message transmission with the 433MHz TX module . 
 
 Adafruit BMP085 library - https://github.com/adafruit/Adafruit-BMP085-Library
 Adafruit DHT sensor library - https://github.com/adafruit/DHT-sensor-library
 VirtualWire library - http://www.open.com.au/mikem/arduino/VirtualWire/index.html

 The PIN connections in the sketch are as - 

 BMP085 - 
           VCC = 3.3V
           GND = ground rail
           SCL = Analog Pin # 5
           SDA = Analog Pin # 4

 DHT11 - 
         VCC  = 5V
         DATA = Digital Pin # 2 (A 10K resistor is used as a pull up)
         GND  = ground rail

 433MHz Tx - 
           TX  = Digital Pin # 12
           VCC = 5V
           GND = ground rail
*/

/* 

  Current version: 0.1
  Version 0.1 - BMP085 + DHT22 + 433MHz Tx
  Version 0.2 - Updated message format, merged all params into one

*/  

#include "Wire.h"
#include "Adafruit_BMP085.h"
#include "DHT.h"
#include "VirtualWire.h"

#define dhtPIN  2
#define DHTTYPE DHT11

const int ledPin = 13;

Adafruit_BMP085 bmp085;
DHT dht(dhtPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    bmp085.begin();
    vw_setup(2000);  // 2000 bits/second
}

void loop() {
    digitalWrite(ledPin, true);
    delay(250);
    digitalWrite(ledPin, false);
    
    String stringToTransmit;
    
    // Read temperature and humidity
    float humidity = dht.readHumidity();
    float temperature_dht = dht.readTemperature();

    if (isnan(temperature_dht) || isnan(humidity)) {
        Serial.println("Failed to read from DHT");
    } else {
        Serial.print("Humidity: "); 
        Serial.print(humidity);
        Serial.println(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(temperature_dht);
        Serial.println(" C");

        // Transmit temperature and humidity
        char chrBufferTp1[10];
        String temperature = "Ta" + String(dtostrf(temperature_dht, 3, 2, chrBufferTp1));
        stringToTransmit += temperature;
 
        char chrBufferHumidity[10];
        String humidity = "H" + String(dtostrf(dht.readHumidity(), 3, 2, chrBufferHumidity));
        stringToTransmit += humidity;        
    }     

    // Transmit 
    // dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
    char test[10];
    String temperature = "Tb" + String(dtostrf(bmp085.readTemperature(), 3, 2, test));
    stringToTransmit += temperature;
        
    String pressure = "P" + String(bmp085.readPressure());
    stringToTransmit += pressure;
        
    // Message has been constructed, now transmit
    char strBufferToTransmit[VW_MAX_MESSAGE_LEN];
    stringToTransmit.toCharArray(strBufferToTransmit, VW_MAX_MESSAGE_LEN);
    vw_send((uint8_t *)strBufferToTransmit, strlen(strBufferToTransmit));
    vw_wait_tx(); // Wait until the message has been sent            
    Serial.print("Transmitted message: ");
    Serial.println(stringToTransmit);
    
    digitalWrite(ledPin, true);
    delay(250);
    digitalWrite(ledPin, false);
    
    delay(10000); // Send every 10 seconds
}



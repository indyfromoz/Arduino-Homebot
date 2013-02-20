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

 This sketch uses a BMP085 temperature+pressure sensor connected to an Arduino along with a 433MHz 
 transmitter. The Adafruit BMP085 library is used for the temperature, pressure and altitude 
 measurements. The VirtualWire library is used for the setup and message transmission with the 
 433MHz TX module . 
 
 Adafruit BMP085 library - https://github.com/adafruit/Adafruit-BMP085-Library
 VirtualWire library - http://www.open.com.au/mikem/arduino/VirtualWire/index.html

 The PIN connections in the sketch are as - 

 BMP085 - 
           VCC = 3.3V
           GND = ground rail
           SCL = Analog Pin # 5
           SDA = Analog Pin # 4

 433MHz Tx - 
           TX  = Digital Pin # 12
           VCC = 5V
           GND = ground rail
*/

#include "Wire.h"
#include "Adafruit_BMP085.h"
#include "VirtualWire.h"

Adafruit_BMP085 bmp085;
const int ledPin = 13;

void setup() {
    Serial.begin(9600);
    bmp085.begin();
    vw_setup(2000);  // 2000 bits/second
}

void loop() {
    digitalWrite(ledPin, true);
    delay(250);
    digitalWrite(ledPin, false);
    
    Serial.print("Temperature = ");
    Serial.print(bmp085.readTemperature());
    Serial.println(" C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp085.readPressure());
    Serial.println(" Pa");
    
    Serial.print("Altitude = ");
    Serial.print(bmp085.readAltitude());
    Serial.println(" meters");
    
    // Transmit 
    // dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
    char test[10];
    String temperature = "T" + String(dtostrf(bmp085.readTemperature(), 3, 2, test));
    char strBufferTemperature[10];
    temperature.toCharArray(strBufferTemperature, 10);
    vw_send((uint8_t *)strBufferTemperature, strlen(strBufferTemperature));
    vw_wait_tx(); // Wait until the message has been sent
    
    String pressure = "P" + String(bmp085.readPressure());
    char strBufferPressure[10];
    pressure.toCharArray(strBufferPressure, 10);
    vw_send((uint8_t *)strBufferPressure, strlen(strBufferPressure));
    vw_wait_tx(); // Wait until the message has been sent    
    
    digitalWrite(ledPin, true);
    delay(250);
    digitalWrite(ledPin, false);
    
    delay(10000); // Send every 10 seconds
}


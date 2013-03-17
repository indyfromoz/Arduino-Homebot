Homebot
=======

Project Homebot is a home automation experiment. It consists of 'bots' that send data such as temperature, pressure & humidity (called *Horai*), presence of gas or fumes or someone in their proximity (called *Itus*) to a mother bot (called *Uranus*). 

*Uranus* displays data received from various bots on a LCD and optionally uploads the data to services like COSM. 


#### Hardware

*Uranus* consists of a Arduino Uno with a Wiznet W5100 Ethernet shield. It has a 433MHz receiver for receiving data from remote bots which is uploaded to a service such as COSM.

Remote units consists of an Arduino Nano v3.0, one or more sensors and a 433MHz transmitter. *Horai* consists of DHT11 or DHT22 for humidity measurement and BMP085 for temperature and pressure measure. 

#### Data 

Three datastreams are available in a COSM feed - <https://cosm.com/feeds/106000>. The data originates from a DHT11 and a BMP085.

#### Compiling _Uranus_

You will need to obtain your own COSM API key to be able to use the COSM transmitter source in _433MHz_Ethernet+COSM_Receiver.ino_. Create a file called _APIKey.h_, define two parameters - 

```
#define API_KEY "YOUR_API_KEY" // Cosm API key
#define FEED_ID YOUR_FEED_ID   // Cosm feed ID
```

Compile and deply the code. The Ethernet cable should be plugged in before the Arduino is powered on. 

Feel free to contact me at - [indyfromoz@gmail.com](indyfromoz@gmail.com) if you have questions. 


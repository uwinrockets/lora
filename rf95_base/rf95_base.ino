// rf95_base server
// Recieves the data from the Payload
// LoRa Simple Client for Arduino :
// Support Devices: LoRa Shield + Arduino

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 rf95;

// Class to manage message delivery and receipt
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

float frequency = 915.0; // Change the frequency here.

void setup() 
{
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

Serial.begin(115200);
while (!Serial) ; // Wait for serial port to be available
Serial.println("LoRa Simple Server");
if (!manager.init())
Serial.println("LoRa init failed");
// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
rf95.setFrequency(frequency);

rf95.setTxPower(13, false);

rf95.setCADTimeout(10000);
Serial.println("Waiting for radio to setup");
delay(1000);
Serial.println("Setup completed");
}

//uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed - this is base");
    }
  }
}



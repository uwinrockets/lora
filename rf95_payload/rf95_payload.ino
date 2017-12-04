// rf95_payload
// Sends data to the Base
// LoRa Simple Server for Arduino :
// Support Devices: LoRa Shield/GPS + Arduino

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 rf95;

// Class to manage message delivery and receipt
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

float frequency = 915.0; // Change the frequency here.

void setup() 
{
  //  pinMode(4, OUTPUT);
  //  digitalWrite(4, HIGH);

  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  Serial.println("LoRa Simple Client");
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

uint8_t data[] = "Hello World ! I'm happy if you can read me";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  Serial.println("Sending to rf95_server");
    
  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("No reply, is rf95_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(500);
}


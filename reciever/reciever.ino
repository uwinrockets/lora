#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

//#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 1

// Singleton instance of the radio driver
RH_ASK driver;
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, SERVER_ADDRESS);

struct dataStruct{
  float tempValue;  //Store temperature sensor value (degC)
  int soilValue;    //Store soil moisture level (%)
  int soilRaw;      //Store hygrometer sensor reading
  unsigned long counter;
   
}SensorReadings;

void setup()
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

  SensorReadings.tempValue = 0;
  SensorReadings.soilValue = 0;
  SensorReadings.soilRaw = 0;
  SensorReadings.counter = 0;
}

void loop()
{
ReciveFromSensors();
}

void ReciveFromSensors()
{
  // Dont put this on the stack:
  uint8_t buf[sizeof(SensorReadings)];
  uint8_t from;
  uint8_t len = sizeof(buf);
 
  if (manager.available())
  {   
    // Wait for a message addressed to us from the client
    if (manager.recvfrom(buf, &len, &from))
    {
      int i;
      memcpy(&SensorReadings, buf, sizeof(SensorReadings));
      Serial.println("--------------------------------------------");
      Serial.print("Got message from unit: ");
      Serial.println(from, DEC);
      Serial.print("Transmission number: ");
      Serial.println(SensorReadings.counter);
      Serial.println("");
       
      Serial.print("Temperature: ");
      Serial.println(SensorReadings.tempValue);
       
      Serial.print("Soil moisture content: ");
      Serial.println(SensorReadings.soilValue);
       
      Serial.print("Hygrometer sensor reading");
      Serial.println(SensorReadings.soilRaw);
      Serial.println("--------------------------------------------");
    }
  }
}

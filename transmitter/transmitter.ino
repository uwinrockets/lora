#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//RH communication
#define CLIENT_ADDRESS 2
#define SERVER_ADDRESS 1
// Singleton instance of the radio driver
RH_ASK driver;
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, CLIENT_ADDRESS);

//OneWire sensors
// Data wire is plugged into port 16 on the Arduino
#define ONE_WIRE_BUS 10
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

struct dataStruct{
  float tempValue;
  int soilValue;
  int soilRaw;
  unsigned long counter;
   
}SensorReadings;

 // RF communication, Dont put this on the stack:
  byte buf[sizeof(SensorReadings)] = {0};

void setup()
{
  Serial.begin(9600);
 
  //RF communication
  if (!manager.init())
    Serial.println("init failed");

  // OneWire sensors
  sensors.begin();

  SensorReadings.tempValue = 0;
  SensorReadings.soilValue = 0;
  SensorReadings.soilRaw = 0;
  SensorReadings.counter = 0;
}

void loop()
{
  Serial.println("------------------------------------");
  float tempValue;
  int soilValue;
 
  temperature(); //Read temp sensor
  delay(1000);
 
  soilValue = soil(); //Read hygrometer
  delay(1000);
 
  SendValues(); //Send sensor values
  delay(2000);
  Serial.println("------------------------------------");
}

//Get temperatures from Dallas sensor
void temperature()
{
  // issue a global temperature request to all devices on the bus
  Serial.print("Requesting temperatures from all...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // Print temperatures
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));
  Serial.println(""); 
  SensorReadings.tempValue = sensors.getTempCByIndex(0);
}

//Soil hygrometer
int soil()
{
 // read soil moisture from sensor
 int sensorValue = analogRead(A1);
 int sensorValueCons = sensorValue;
 
 sensorValueCons = constrain (sensorValue, 300,1023);
 //Sensor value 300 equals 100% humidity and sensor value 1023 equals 0% humidity

 SensorReadings.soilValue = map (sensorValueCons, 300, 1023, 100, 0);
 Serial.print ("Soil hygrometer: ");
 Serial.print (SensorReadings.soilValue);
 Serial.println ("%");
 //Print sensor value
 Serial.print ("Soil hygrometer sensor value: ");
 Serial.println (sensorValue);
 Serial.println("");
 SensorReadings.soilRaw = sensorValue;
}

//RF communication
void SendValues()
{ 
  //Load message into data-array
  //byte data[sizeof(sensorValues)];

  byte zize=sizeof(SensorReadings);
  memcpy (buf, &SensorReadings, zize);

 
  Serial.println("Sending to ask_datagram_server");
  int i = 0;
  int repeat = 10; //Times to repeat same message
   
  // Send a message to manager_server
  while (i<repeat)
  {
    if (manager.sendto(buf, zize, SERVER_ADDRESS))
    {
      Serial.println("Message sent");   
    }
    else
    {
      Serial.println("sendto failed");
    }
    delay(100);
    i = i+1;
  }
  SensorReadings.counter = SensorReadings.counter + 1;
}

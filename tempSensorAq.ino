#include "Communication.h"
#include "Controller.h"
#include "Sensor.h"
#include "Settings.h"
#include "EEPROM.h"
#include "DHT.h"
#include "SoftwareSerial.h"

#define DHTPIN A1     // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
#define READTIME 1200 //20 minutes in secs
#define WRITETIME 3600  //1h in secs
#define INDICATOR LED_BUILTIN
#define RST_BTN 6

//Variables
int readTicks;
int writeTicks;

//Objects
DHT dht(DHTPIN, DHTTYPE);
Sensor sensor;
Controller controller;
Settings settings;
Communication communication;

void setup() 
{
  digitalWrite(RST_BTN, HIGH);
  Serial.begin(19200);
  Serial.println("Setup Started");
  initializeVariables(); 
  controller.initialize();
  settings.initialize();
  communication.begin();
  if(communication.initializeServer(settings.getCode(1)))
    resetFunc();
  if(communication.initializeClient(&settings))
    resetFunc();
  if(communication.readDatabase(&settings))
    resetFunc();
  sensor.initialize(&dht);
  Indicator(LOW);
}

void initializeVariables()
{
  readTicks=0;
  writeTicks=0;
  pinMode(INDICATOR,OUTPUT); 
  pinMode(RST_BTN,OUTPUT); 
  
  Serial.println("Variables Initialized");
}

void Indicator(bool error)
{
  digitalWrite(INDICATOR,!error);
}


/*void serialEvent() 
{
    /*communication.waitForData();
    Serial.println("Timer Initialized");
}*/

void resetFunc()
{
  digitalWrite(RST_BTN, LOW);
}

void loop() 
{
  if(sensor.measure(dht))
    resetFunc();
  controller.set(sensor.getLastMeasure(),settings.getMinTemp(),settings.getMaxTemp());
  
  //Tests if readTicks have achieved the READTIME
  if((readTicks>=READTIME)&&(sensor.getLastMeasure()!=-99)) 
  {
    if(communication.readDatabase(&settings))
      resetFunc();
    readTicks=0;
  }

  //Tests if writeTicks have achieved the WRITETIME
  else if((writeTicks>=WRITETIME)&&(sensor.getLastMeasure()!=-99))
  {
    if(communication.writeDatabase(settings,sensor.getLastMeasure(), controller.getCoolerStatus(),controller.getHeaterStatus()))
      resetFunc();
    writeTicks=0;
  }

  if(communication.waitForData(&settings))
      if(communication.initializeClient(&settings))
        resetFunc();

  delay(5000);
  readTicks+=5;
  writeTicks+=5;
}


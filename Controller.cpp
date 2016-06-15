#include "Controller.h"

#define cooler 4
#define heater 5


Controller::Controller(void){}

void Controller::initialize(void)
{
  //Set digital pins mode
  pinMode(cooler,OUTPUT);
  pinMode(heater,OUTPUT); 

  //Set digital pins state
  digitalWrite(cooler,HIGH);
  digitalWrite(heater,HIGH);
   
  coolerStatus=HIGH;
  heaterStatus=HIGH;

  Serial.println("Controller Initialized");
}
  
//Verifies the adequated status for each output
void Controller::set(float curTemp, float maxTemp, float minTemp)
{
  //Calculates medium temperature and allowed error
  float allowedError=(maxTemp-minTemp)/20;  //10% error allowed
  float mediumTemp=(maxTemp+minTemp)/2;
  float maxErrorTemp=mediumTemp+allowedError;
  float minErrorTemp=mediumTemp-allowedError;
  
  //Verifies the adequated status of each output
  if((curTemp>maxErrorTemp)&&(curTemp!=-99))
  {
    digitalWrite(cooler,LOW);
    digitalWrite(heater,HIGH);

    coolerStatus=LOW;
    heaterStatus=HIGH;
  }
  else if((curTemp<minErrorTemp)&&(curTemp!=-99))
  {
    digitalWrite(cooler,HIGH);
    digitalWrite(heater,LOW);

    coolerStatus=HIGH;
    heaterStatus=LOW;
  }
  else
  {
    digitalWrite(cooler,HIGH);
    digitalWrite(heater,HIGH);
  
    coolerStatus=HIGH;
    heaterStatus=HIGH;
  }
}

bool Controller::getCoolerStatus(void)
{
  return coolerStatus;
}

bool Controller::getHeaterStatus(void)
{
  return heaterStatus;
}

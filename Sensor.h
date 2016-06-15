/* Sensor library

written by LucasNMilagres
*/
#ifndef Sensor_H
#define Sensor_H

#include "Arduino.h"
#include "DHT.h"

class Sensor
{    
  private:
  float lastMeasure;
  
  public:
   Sensor();
   void initialize(DHT* dht);
   int measure(DHT dht); 
   float getLastMeasure(void);
};

#endif

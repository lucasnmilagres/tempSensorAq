#include "Sensor.h"

Sensor::Sensor(void) {}

void Sensor::initialize(DHT* dht)
{
  lastMeasure=-99;
  dht->begin();

  Serial.println("Sensor Initialized");
}
  
int Sensor::measure(DHT dht) 
{
  float t = dht.readTemperature();
    
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    lastMeasure=-99;
    return 1;
  }
  else
  {
    Serial.print("Measure: ");
    Serial.print(t);
    Serial.println(" *C ");
    lastMeasure=t;
    return 0;
  }
}

float Sensor::getLastMeasure(void)
{
   return lastMeasure;
}


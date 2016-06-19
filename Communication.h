/* Communication library

written by LucasNMilagres
*/
#ifndef Communication_H
#define Communication_H

#include "Arduino.h"
#include "Sensor.h"
#include "Settings.h"
#include "SoftwareSerial.h"

class Communication
{
  private:
  int error;
  String acIP;
  String serverIP;
  
  public:
  Communication(void);
  void begin();
  int initializeServer(String code);
  int waitForData(Settings* settings);
  int initializeClient(Settings* settings);
  int writeDatabase(Settings settings,float curTemp, bool coolerStatus,bool heaterStatus);
  int readDatabase(Settings* settings);
  String sendData(String command, String terminator="OK",String starter="", int cicles=10, int timeout=1000); 
};

#endif

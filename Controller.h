/* Controller library

written by LucasNMilagres
*/
#ifndef Controller_H
#define Controller_H

#include "Arduino.h"

class Controller
{
  private:
  bool coolerStatus;
  bool heaterStatus;

  public:
  Controller(void);
  void initialize(void);
  void set(float curTemp, float maxTemp, float minTemp);
  bool getCoolerStatus(void);
  bool getHeaterStatus(void);
};

#endif

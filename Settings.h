/* Settings library

written by LucasNMilagres
*/
#ifndef Settings_H
#define Settings_H

#include "Arduino.h"
#include "EEPROM.h"

class Settings
{
  private:
  String INITIALIZED;
  float maxTemp;
  float minTemp;
  String code;
  String ssid;
  String password;
  const static int BUFSIZE = 50;
  const String FACTORY="CREELED";
  const String PREFIX="TM";
  char buf[BUFSIZE];

  public:
  Settings(void);
  void initialize();
  float getMaxTemp(void);
  float getMinTemp(void);
  String getSSID(void);
  String getPassword(void);
  void setSSID(String _ssid);
  void setPassword(String _password);
  String getCode(bool factory=0);
  void setCode(String _code);
  void setLimits(float maxTemp, float minTemp);
  bool writeStringEEPROM(int addr, const char* string);
  bool writeBytesEEPROM(int startAddr, const byte* array, int numBytes);
  bool readStringEEPROM(int addr, char* buffer, int bufSize);
  String isInitialized(void);
  void readEEPROM(void);
  void software_Reboot(void);
};

#endif

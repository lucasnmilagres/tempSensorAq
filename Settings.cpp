#include "Settings.h"

#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 511
#define RST_BTN 6

Settings::Settings(void){}

void Settings::initialize(void)
{
  readStringEEPROM(0,buf,2);
  INITIALIZED=buf;
  if(INITIALIZED=="1")
  {
    readStringEEPROM(2,buf,BUFSIZE);
    code=buf;
    readStringEEPROM(52, buf, BUFSIZE);
    ssid=buf;
    readStringEEPROM(102, buf, BUFSIZE);
    password=buf;
    Serial.print("Device:"); 
  }
  else
  {
    code=PREFIX+"000";
    Serial.print("New Device:");
  }

  Serial.println(code);
  Serial.println("Settings Initialized");
}

float Settings::getMaxTemp(void)
{
  return maxTemp;
}

float Settings::getMinTemp(void)
{
  return minTemp;
}

String Settings::getCode(bool factory)
{
  if(factory)
    return FACTORY+code;
  else
    return code;
}

void Settings::setCode(String _code)
{
  code=_code;
}

String Settings::getSSID(void)
{
  return ssid;
}

String Settings::getPassword(void)
{
  return password;
}

void Settings::setSSID(String _ssid)
{
  ssid=_ssid;
}

void Settings::setPassword(String _password)
{
  password=_password;
}

void Settings::setLimits(float _maxTemp, float _minTemp)
{
  maxTemp=_maxTemp;
  minTemp=_minTemp;
}

String Settings::isInitialized(void)
{
  return INITIALIZED;
}

/*Writes a sequence of bytes to eeprom starting at the specified address.
  Returns true if the whole array is successfully written.
  Returns false if the start or end addresses aren't between
  the minimum and maximum allowed values.
  When returning false, nothing gets written to eeprom.*/
bool Settings::writeBytesEEPROM(int startAddr, const byte* array, int numBytes) 
{
  // counter
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range 
  /*if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) 
      return false;*/
  
  for (i = 0; i < numBytes; i++) 
      EEPROM.write(startAddr + i, array[i]);
  
  return true;
}

/*Writes a string starting at the specified address.
  Returns true if the whole string is successfully written.
  Returns false if the address of one or more bytes fall outside the allowed range.
  If false is returned, nothing gets written to the eeprom.*/
bool Settings::writeStringEEPROM(int addr, const char* string)
{
  int numBytes; // actual number of bytes to be written

  //write the string contents plus the string terminator byte (0x00)
  numBytes = strlen(string) + 1;

  return writeBytesEEPROM(addr, (const byte*)string, numBytes);
}

/*Reads a string starting from the specified address.
  Returns true if at least one byte (even only the string terminator one) is read.
  Returns false if the start address falls outside the allowed range or declare buffer size is zero. 
  The reading might stop for several reasons:
  - no more space in the provided buffer
  - last eeprom address reached
  - string terminator byte (0x00) encountered.*/
bool Settings::readStringEEPROM(int addr, char* buffer, int bufSize) 
{
  byte ch; // byte read from eeprom
  int bytesRead; // number of bytes read so far

  /*if (!eeprom_is_addr_ok(addr)) // check start address
    return false;*/

  if (bufSize == 0) // how can we store bytes in an empty buffer ?
    return false;

  // is there is room for the string terminator only, no reason to go further
  if (bufSize == 1) 
  {
    buffer[0] = 0;
    return true;
  }

  bytesRead = 0; // initialize byte counter
  ch = EEPROM.read(addr + bytesRead); // read next byte from eeprom
  buffer[bytesRead] = ch; // store it into the user buffer
  bytesRead++; // increment byte counter

  /*Stop conditions:
    - the character just read is the string terminator one (0x00)
    - we have filled the user buffer
    - we have reached the last eeprom address*/
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) )
  {
    // if no stop condition is met, read the next byte from eeprom
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch; // store it into the user buffer
    bytesRead++; // increment byte counter
  }

  // make sure the user buffer has a string terminator, (0x00) as its last byte
  if ((ch != 0x00) && (bytesRead >= 1)) 
    buffer[bytesRead - 1] = 0;

  return true;
}


void Settings::software_Reboot(void)
{
  digitalWrite(RST_BTN, LOW);
}


#include "Communication.h"

#define DEBUG 1

SoftwareSerial esp8266(2,3); //RX pino 2, TX pino 3

Communication::Communication(void){}

void Communication::begin(void)
{
  acIP="192.168.5.1";
  serverIP="192.168.43.25";
}
  
int Communication::initializeServer(String code)
{
  esp8266.begin(19200);
  error=0;
  sendData("AT+RST\r\n"); // reset module
  sendData("AT+CWSAP_DEF=\""+code+"\",\"password\",3,0\r\n"); //Set ssid to code;
  sendData("AT+RST\r\n"); // reset module
  sendData("AT+CWMODE_DEF=3\r\n"); // configure as access point+Station
  sendData("AT+CIPMUX=1\r\n");  // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n"); // turn on server on port 80 
  sendData("AT+CIPAP_DEF=\""+acIP+"\"\r\n"); // set ip addr of ESP8266 softAP.

  if(error)
    Serial.println("Access Point Initialize Error!");
  else
    Serial.println("Access Point Initialized");
  return error;
}

int Communication::waitForData(Settings* settings)
{
  if (esp8266.find("+IPD,"))
  {
    delay(300);
    String request="";

    long int time = millis();
    while ( ((time + 1000) > millis())&&(strlen(request.c_str())<=100))
    {
      while (esp8266.available()&&(strlen(request.c_str())<=100))
      {
        char letter=esp8266.read();
        request+=(char)letter;
      }
    }

    /*Serial.println("*******************************Start*************************");
    Serial.println(request);
    Serial.println("*******************************Finish*************************");*/

    int codeStartIndex=request.indexOf("Code=");
    int codeEndIndex=request.indexOf(";");
    int ssidStartIndex=request.indexOf("SSID=");
    int ssidEndIndex=request.indexOf(";",ssidStartIndex);
    int passwordStartIndex=request.indexOf("Password=");
    int passwordEndIndex=request.indexOf("%3C!");
    

    if((codeStartIndex!=-1)&&(codeEndIndex!=-1))
    {
      //Serial.println("SSID:"+request.substring(ssidStartIndex+5,ssidEndIndex));
      settings->setCode(request.substring(codeStartIndex+5,codeEndIndex));
    }
    if((ssidStartIndex!=-1)&&(ssidEndIndex!=-1))
    {
      //Serial.println("SSID:"+request.substring(ssidStartIndex+5,ssidEndIndex));
      settings->setSSID(request.substring(ssidStartIndex+5,ssidEndIndex));
    }
    if((passwordStartIndex!=-1)&&(passwordEndIndex!=-1))    
    {
      //Serial.println("PASSWORD:"+request.substring(passwordStartIndex+9,passwordEndIndex));
      settings->setPassword(request.substring(passwordStartIndex+9,passwordEndIndex));
    }
    
    return 1;
  }
  return 0;
}

int Communication::initializeClient(Settings* settings)
{  
    do
    {
      waitForData(settings);
      error=0;
      sendData("AT+CWJAP_DEF=\""+settings->getSSID()+"\",\""+settings->getPassword()+"\"\r\n","OK","",1,500);
    }while(error>0); //  connect to router
    
    settings->writeStringEEPROM(52, settings->getSSID().c_str()); 
    settings->writeStringEEPROM(102, settings->getPassword().c_str()); 

  if(settings->isInitialized()!="1")
  {
    settings->writeStringEEPROM(0, "1"); 
    settings->writeStringEEPROM(2, settings->getCode().c_str()); 
     settings->software_Reboot();
  }

  if(error)
    Serial.println("Client Initialize Error!");
  else
    Serial.println("Client Initialized");
  return error;
}

int Communication::writeDatabase(Settings settings,float curTemp, bool coolerStatus,bool heaterStatus)
{ 
  error=0;
  
  String code=settings.getCode();

  String msg="GET ";
  msg+="/CreeLed/SendCurTemp.php?";
  msg+="DC=";
  msg+=code;
  msg+="&";
  msg+="Temp=";
  msg+=String(curTemp,2);
  msg+="&";
  msg+="CS=";
  msg+=String(coolerStatus);
  msg+="&";
  msg+="HS=";
  msg+=String(heaterStatus);
  msg+=" HTTP/1.0\r\n";
  msg+="Connection: Keep-Alive\r\n\r\n";
  sendData("AT+CIPSTART=4,\"TCP\",\""+serverIP+"\",80\r\n");
  sendData("AT+CIPSEND=4,"+String(strlen(msg.c_str()))+"\r\n", ">");
  sendData(msg,"New settings registered!","{");
  
  sendData("AT+CIPCLOSE=4\r\n");
  return error;
}

int Communication::readDatabase(Settings* settings)
{  
  error=0;
  
  String code=settings->getCode();
  
  String msg="GET ";
  msg+="/CreeLed/GetTempLimits.php?";
  msg+="DC=";
  msg+=code;
  msg+=" HTTP/1.0\r\n";
  msg+="Connection: Keep-Alive\r\n\r\n";
  sendData("AT+CIPSTART=4,\"TCP\",\""+serverIP+"\",80\r\n");
  sendData("AT+CIPSEND=4,"+String(strlen(msg.c_str()))+"\r\n", ">");
  String resposta;
  resposta=sendData(msg,"}","{");
  sendData("AT+CIPCLOSE=4\r\n");
  resposta.replace('.',',');
  int maxTempStartIndex=resposta.indexOf("\"Limits\":[\"");
  int separatorIndex=resposta.indexOf("\",\"");
  int minTempEndIndex=resposta.indexOf("\"]");

  settings->setLimits(atof(resposta.substring(maxTempStartIndex+11,separatorIndex).c_str()),atof(resposta.substring(separatorIndex+3,minTempEndIndex).c_str()));
  /*Serial.println(settings->getMaxTemp());
  Serial.println(settings->getMinTemp());*/
  return error;
}

String Communication::sendData(String command, String terminator, String starter, int cicles,int timeout)
{
  long int time = millis();
  String response = "";
  for(int count=0;count<cicles;count++)
  {
  // Envio dos comandos AT para o modulo
  //esp8266.print(command);
  esp8266.print(command);
  long int start = millis();
  char buf[15];
  starter.toCharArray(buf,15);
  esp8266.find(buf);
  while ( (start + timeout) > millis())
  {
    //while (esp8266.available())
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      //char c = esp8266.read(); // read the next character.
      char c = esp8266.read(); // read the next character.
      response.concat(c);
    }
  }
  if (DEBUG)
  {
    Serial.print(response);
  }

    if((response.indexOf(terminator)!=-1)&&(terminator!=""))
        return response;
  }

  error++;
  return response;
}




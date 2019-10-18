#include "Arduino.h"
#include "SoftwareSerial.h"
#include "ESP8266.h"

ESP8266Class::ESP8266Class() : esp8266(NULL)
{
  flagSS = false;
}

void ESP8266Class::init(long baud = 115200, uint8_t RX = 10, uint8_t RT = 11)
{
  if (nullptr != esp8266)
  {
    delete esp8266;
  }
  esp8266 = new SoftwareSerial(RX_PIN = RX, TX_PIN = RT);
  esp8266->begin(baud);
}

void ESP8266Class::begin(String accessPoint, String password) {
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + accessPoint + "\",\"" + password + "\"", 20, "OK");
}

void ESP8266Class::sendData(String host, String port, String data) {
  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + host + "\"," + port, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(data.length() + 4), 4, ">");
  esp8266->println(data);
  delay(1500);
  sendCommand("AT+CIPCLOSE=0", 5, "OK");
}

boolean ESP8266Class::sendCommand(String command, int maxTime, char readReplay[])
{
  int countTimeCommand = 0;
  bool isFound = false;
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266->println(command);     //at+cipsend
    if (esp8266->find(readReplay)) //ok
    {
      isFound = true;
      break;
    }

    countTimeCommand++;
  }

  if (isFound)
  {
    Serial.println("OYI");
    countTimeCommand = 0;
  } 
  else 
  {
    Serial.println("Fail");
  }
}

ESP8266Class::~ESP8266Class()
{
  if (flagSS)
  {
    delete esp8266;
  }
}

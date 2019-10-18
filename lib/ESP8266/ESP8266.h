#ifndef __ESP8266_H
#define __ESP8266_H

#include <SoftwareSerial.h>

class ESP8266Class
{
private:
	uint8_t RX_PIN = 10; // RX
	uint8_t TX_PIN = 11; // TX
	boolean flagSS;
	SoftwareSerial *esp8266 = nullptr;
	boolean sendCommand(const String &command, const int &maxTime, char readReplay[]);

public:
	ESP8266Class();
	void init(long baud, uint8_t RX, uint8_t TX);
	void begin(const String &AP, const String &password);
	void sendData(const String &host, const String &port,const String &data);
	~ESP8266Class();
};

#endif //__ESP8266_H
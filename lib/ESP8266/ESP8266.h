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
	boolean sendCommand(String command, int maxTime, char readReplay[]);

public:
	ESP8266Class();
	void init(long baud, uint8_t RX, uint8_t TX);
	void begin(String AP, String password);
	void sendData(String host, String port, String data);
	~ESP8266Class();
};

#endif //__ESP8266_H
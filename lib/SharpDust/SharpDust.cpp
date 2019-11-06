#include "Arduino.h"
#include "SharpDust.h"

#define SAMPLING_TIME	280
#define SLEEP_TIME		9680

SharpDustClass::SharpDustClass()
{
}

void SharpDustClass::begin(uint8_t lPin, uint8_t mPin)
{
	Serial.println("LedPIN=");
	ledPin = lPin;
	measurePin = mPin;
	pinMode(ledPin, OUTPUT);
	pinMode(A5, INPUT);
	digitalWrite(ledPin, HIGH);

	Serial.print(ledPin);
	Serial.println("");
}

float SharpDustClass::getDrawMeasure(const int &SAMPLING_NUM)
{
	int sum = 0;
	for (int i = 0; i < SAMPLING_NUM; i++)
	{
		digitalWrite(ledPin, LOW);

		// Wait 0.28ms before taking a reading of the output voltage as per spec.
		delayMicroseconds(SAMPLING_TIME);

		// Record the output voltage. This operation takes around 100 microseconds.
		sum += analogRead(measurePin);

		// Turn off the dust sensor LED by setting digital pin HIGH.
		digitalWrite(ledPin, HIGH);

		// Wait for remainder of the 10ms cycle = 10000 - 320 microseconds.
		delayMicroseconds(SLEEP_TIME);
	}
	return sum *1.0/ SAMPLING_NUM;
}

SharpDustClass SharpDust;
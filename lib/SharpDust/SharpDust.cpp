#include "Arduino.h"
#include "SharpDust.h"

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

float SharpDustClass::getDrawMeasure(int SAMPLING_NUM)
{
	int sum = 0;
	for (int i = 0; i < SAMPLING_NUM; i++)
	{
		digitalWrite(ledPin, LOW);

		// Wait 0.28ms before taking a reading of the output voltage as per spec.
		delayMicroseconds(__SHARP_DUST_SAMPLING_TIME);

		// Record the output voltage. This operation takes around 100 microseconds.
		sum += analogRead(measurePin);
		// delay to make sure!
		//  delayMicroseconds(40);
		// Turn off the dust sensor LED by setting digital pin HIGH.
		digitalWrite(ledPin, HIGH);

		// Wait for remainder of the 10ms cycle = 10000 - 280 - 100 microseconds.
		delayMicroseconds(__SHARP_DUST_SLEEP_TIME);
		// delayMicroseconds(9680);
	}
	return sum *1.0/ SAMPLING_NUM;
}

SharpDustClass SharpDust;
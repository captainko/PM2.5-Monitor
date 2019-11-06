#ifndef __SHARP_DUST_H
#define __SHARP_DUST_H

class SharpDustClass
{
	private:
		uint8_t ledPin;			//Digital pin for LED power
		uint8_t measurePin; 	//Analog pin for measurement
		
	public:
		SharpDustClass();
		void begin(uint8_t ledPin, uint8_t mPin);
		float getDrawMeasure(const int &SAMPLING_NUM = 1);
};

extern SharpDustClass SharpDust;
#endif	//__SHARP_DUST_H
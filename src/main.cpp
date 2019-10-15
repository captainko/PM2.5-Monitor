/////////////////////////////////////////////////////////////////////////////
// Sharp GP2Y1014AU0F Dust Sensor Demo
//
// Board Connection:
//  +GP2Y1014    Arduino
//   V-LED       Between R1 and C1
//   LED-GND     C1 and GND
//   LED         Pin 12
//   S-GND       GND
//   Vo          A5
//   Vcc         5V
// +LCD(16x2)   Arduino
//   Vdd         5V
//   Vss         GND
//   Rs          Pin 8
//   E           Pin 9
//   D4          Pin 4
//   D5          Pin 5
//   D6          Pin 6
//   D7          Pin 7
// Serial monitor setting:
//   9600 baud
/////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SharpDust.h>

#define USE_AVG
#define USE_LCD
#define PRINT_RAW_DATA
#define SLEEP_TIME 1000 // ms

#ifdef USE_LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#endif //~USE_LCD

// Arduino pin numbers.
#define LEDPin 12    //  pin D12 -> sensor LED.
#define sensorPin A5 //  pin A5  -> sesor Vout.

#ifdef USE_AVG
#define MEASURE_N 10
#endif //USE_AVG

#ifdef USE_LCD
#define LEVEL short int
#endif //~USE_LCD

// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;

// Set the typical output voltage in Volts when there is zero dust.
static float Voc = 0.6;

// Timers // there is always 10ms sleep every loops because of the sensor cycle
// helpers

void printValue(String text, unsigned int value, bool isLast = false)
{
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  if (!isLast)
  {
    Serial.print(", ");
  }
}

void printFValue(String text, float value, String units, bool isLast = false)
{
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  Serial.print(units);
  if (!isLast)
  {
    Serial.print(", ");
  }
}

#ifdef USE_LCD
void print_message(float density);
#endif //~USE_LCD

#ifdef USE_AVG
float calAverageRawVo(int m);
#endif // USE_AVG

// ~helpers

void setup()
{
  SharpDust.begin(LEDPin, sensorPin);
  Serial.begin(9600);

#ifdef USE_LCD
  lcd.begin(16, 2);
#endif //~USE_LCD
  delay(2000);
}

void loop()
{
  float VoRaw = SharpDust.getDrawMeasure(10);

#ifdef USE_AVG
  VoRaw = calAverageRawVo(VoRaw);
#endif
  // Compute the output voltage in Volts.
  float Vo = VoRaw / 1024.0 * 5.0;
  printFValue("Vo", Vo * 1000, "mV");

  // Convert to Dust Density in units of ug/m3.
  float dV = Vo - Voc;
  if (dV < 0)
  {
    dV = 0;
    Voc = Vo;
  }
  float density = dV / K * 100.0;
  printFValue("DustDensity", density, "ug/m3", true);

  Serial.println("");

#ifdef USE_LCD
  print_message(density);
#endif //~ USE_LCD
  delay(SLEEP_TIME);
}

#ifdef USE_AVG
float calAverageRawVo(int currentVo)
{
  static int flag_first = 0;
  static float sum, _buff[MEASURE_N];
  static const int _buff_max = MEASURE_N;
  int i;

  if (flag_first == 0)
  {
    flag_first = 1;
    _buff[0] = currentVo;
    for (i = 1, sum = currentVo; i < _buff_max; i++)
    {
      _buff[i] = SharpDust.getDrawMeasure(10);
      Serial.println(_buff[i]);
      sum += _buff[i];
    }
  }
  else
  {
    sum -= _buff[0];
    for (i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[_buff_max - 1] = currentVo;
    sum += currentVo;
  }

  return (float)sum / _buff_max;
}
#endif // USE_AVG

#ifdef USE_LCD
void print_message(float density)
{
  static LEVEL curLevel = -1;

  // calculate pollution level
  static auto pollution_level = [](float x) {
    if (x <= 35)
      return 0;
    if (x <= 75)
      return 1;
    if (x <= 115)
      return 2;
    if (x <= 150)
      return 3;
    if (x <= 250)
      return 4;
    return 5;
  };

  // print the dust level on lcd screen
  static auto print_level = [](const char *level) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(level);
  };

  LEVEL level = pollution_level(density);
  lcd.setCursor(0, 1);
  lcd.print(density);
  lcd.setCursor(6, 1);
  lcd.print("(ug/m3)");
  if (level == curLevel)
    return;

  curLevel = level;
  switch (level)
  {
  case 0:
  {
    print_level("Excellent");
    break;
  }
  case 1:
  {
    print_level("Average");
    break;
  }
  case 2:
  {
    print_level("Light Pollution");
    break;
  }
  case 3:
  {
    print_level("Moderate Pollution");
    break;
  }
  case 4:
  {
    print_level("Heavy Pollution");
    break;
  }
  case 5:
  {
    print_level("Hazardous");
    break;
  }
  }
}
#endif //~USE_LCD
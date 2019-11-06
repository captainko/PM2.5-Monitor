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
//   Rs          Pin 2
//   E           Pin 3
//   D4          Pin 4
//   D5          Pin 5
//   D6          Pin 6
//   D7          Pin 7

// +Esp8266     Arduino
//  RX           Pin 11
//  TX           Pin 10
//  GND          GND
//  VCC          3.3v
//  CH_PD        3.3v

// Serial monitor setting:
//   9600 baud
/////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SharpDust.h>
#include <Waiter.h>

// Utils
#define USE_AVG
#define USE_LCD
#define USE_WIFI
#define PRINT_RAW_DATA
#define SLEEP_TIME 1000 // ms

// Arduino pin numbers.
#define LEDPin 12    //  pin D12 -> sensor LED.
#define sensorPin A5 //  pin A5  -> sesor Vout.

#ifdef USE_AVG
#define MEASURE_N 10
#endif //~USE_AVG

#ifdef USE_LCD
#include <LiquidCrystal.h>
#define LEVEL short int
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
#endif //~USE_LCD

#ifdef USE_WIFI
// #include <SoftwareSerial.h>
#include <ESP8266.h>

#define WIFI_RX 11
#define WIFI_TX 10  
#define ssid "High"
#define pass "thongdeptrai"

ESP8266Class WiFi;
const String AP = "High";           // your network SSID (name)
const String PASS = "thongdeptrai"; // your network SSID (name)
// const String AP = "uittest";
// const String PASS = "uittest123";

// const char pass[] = "thongdeptrai"; // your network password

String API = "XSETYBPX2SC95C5P"; // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand;
boolean found = false;
int valSensor = 1;

// Initialize the Ethernet client object

#endif //~USE_WIFI

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
void print_message(const float &density);
#endif //~USE_LCD

#ifdef USE_AVG
float calAverageRawVo(const int &m);
#endif // USE_AVG

#ifdef USE_WIFI

#endif

// ~helpers

void setup()
{
  SharpDust.begin(LEDPin, sensorPin);
  Serial.begin(9600);

#ifdef USE_LCD
  lcd.begin(16, 2);
#endif //~USE_LCD

#ifdef USE_WIFI
  // initialize serial for ESP module
  WiFi.init(115200, WIFI_RX, WIFI_TX);
  WiFi.begin(AP, PASS);
#endif //~USE_WIFI

  // delay(2000);
}

void loop()
{
  static float densitySum = 0;
  static float densityCount = 0;
  static Waiter delaySensor(1000);
  if (delaySensor.isFinished())
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
    densitySum += density;
    densityCount++;
    printFValue("DustDensity", density, "ug/m3", true);

    Serial.println("");

#ifdef USE_LCD
    print_message(density);
#endif //~ USE_LCD

    delaySensor.reset();
  }

#ifdef USE_WIFI
  static Waiter delaySendData(10000);
  if (delaySendData.isFinished())
  {
    WiFi.sendData(HOST, PORT, "GET /update?api_key=" + API + "&" + field + "=" + String(densitySum / densityCount));
    densityCount = 0;
    densitySum = 0;
    delaySendData.reset();
  }
#endif
}

#ifdef USE_AVG
float calAverageRawVo(const int &currentVo)
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
enum PM25_LEVELS : LEVEL
{
  Excellent,
  Average,
  Light,
  Moderate,
  Heavy,
  Hazardous
};
void print_message(const float &density)
{
  static LEVEL curLevel = -1;
  // calculate pollution level
  static auto pollution_level = [](const float &x) {
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

  // skip printing levels 
  if (level == curLevel)
    return;

  curLevel = level;
  switch (level)
  {
  case Excellent:
    print_level("Excellent");
    break;
  case Average:
    print_level("Average");
    break;
  case Light:
    print_level("Light Pollution");
    break;
  case Moderate:
    print_level("Moderate Pollution");
    break;
  case Heavy:
    print_level("Heavy Pollution");
    break;
  case Hazardous:
    print_level("Hazardous");
  }
}
#endif //~USE_LCD

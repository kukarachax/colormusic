#define FFT_SIZE 64  // 32/64/128/256/512

#define LOWPASSADD 2

#define RESET_SETTINGS false
#define DEBUG false

#define SOUND_R_FREQ 34
#define SOUND_R 35

byte LOW_PASS_ADD = 20;           // "добавочная" величина к нижнему порогу, для надёжности (режим VU)
byte LOW_PASS_FREQ_ADD = 6;       // "добавочная" величина к нижнему порогу, для надёжности (режим частот)
uint16_t LOW_PASS = 100;          // нижний порог шумов режим VU, ручная настройка
uint16_t SPEKTR_LOW_PASS = 40;    // нижний порог шумов режим спектра, ручная настройка
float SMOOTH = 0.3;               // коэффициент плавности анимации VU (по умолчанию 0.5)
#define EXP 1.9                   // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)
#define MAX_COEF 1.8              // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)
#define SMOOTH_STEP 20             // шаг уменьшения яркости в режиме цветомузыки (чем больше, тем быстрее гаснет)
#define LIGHT_SMOOTH 2

#define LOW_COLOR HUE_RED         // цвет низких частот
#define MID_COLOR HUE_YELLOW       // цвет средних
#define HIGH_COLOR HUE_GREEN      // цвет высоких
#define EMPTY_COLOR HUE_PURPLE     // цвет пустых

#define STRIP_PIN 22
#define NUM_LEDS 182
#define STRIP_CHIP WS2812
#define STRIP_COLOR GRB
#define COLOR_COUNT 3

#define STRIPE NUM_LEDS / 2
float freq_to_stripe = NUM_LEDS / 40; //3 режим

#define GMT 3 
#define UPDATE_TIME 5 //seconds
int START_WAKE_UP_MIN = 30;
int FINISH_WAKE_UP_MIN = 10;
int WAKE_UP_FADE_TIME = 5;

#include "FastLED.h"
#include "EEPROM.h"

#include "FFT_C.h"

#include "WiFi.h"
/*
#include "WiFiUdp.h"
#include "NTPClient.h"
*/
#include "WiFiClient.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoOTA.h"


//VolAnalyzer low, high;
CRGB leds[NUM_LEDS];
WiFiServer server(80); 
AsyncWebServer Aserver(80);
/*
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", (GMT * 3600));
*/

#define ssidAP "ColorMusic"
#define passwordAP ""

DEFINE_GRADIENT_PALETTE(soundlevel_gp) {
  0,    0,    255,  0,  // green
  100,  255,  255,  0,  // yellow
  150,  255,  100,  0,  // orange
  200,  255,  50,   0,  // red
  255,  255,  0,    0   // red
};
CRGBPalette32 myPal = soundlevel_gp;

float SmoothFreq = 0.8;       
float MaxCoefFreq = 1.5;      
byte RunningFreqSpeed = 0; 
int RainbowPeriod = 1;
float RainbowLoopStep = 0.5;
int RainbowFadeDelay = 5;
int EmLightsDelay = 20;
int pulseDelay = 20;
byte pulseHue = 255;
int randomMarchDelay = 20;
byte Mode=1;
bool OnOff=true;
byte Bright=250;
byte EmptyBright=30;
byte Shue, Ssat;
byte StrHue, StrSat;
int StrPeriod = 5;
long sleepTime = 0;
unsigned long sleepTimeLast;

int idex = 0;
int idexEmLights = 0;
int Rlenght, Llenght;
int bouncedirection = 0;
int ibright = 0;
int maxLevel = 100;
bool strFlag;
int ledsX[NUM_LEDS][3];
int idexEM = 0;
int MAX_CH = NUM_LEDS / 2;
int TOP_INDEX = int(NUM_LEDS / 2);
float indexCH = (float)255 / MAX_CH;
float averageLevel = 50;
int ihue = 0;
byte Fsat = 255;
byte count;
int raw[FFT_SIZE];
int spectr[FFT_SIZE];
int colorMusic[3];
bool colorMusicFlash[3];
bool running_flag[3];
int thisBright[3];
float colorMusic_f[3], colorMusic_aver[3];
unsigned long running_timer, rainbow_timer;
unsigned long fade_timer, strobe_timer;
unsigned long randomMarchLast, EmLightsLast;
unsigned long pulseTimerLast;
int freq_f[32];
float freq_max_f;
int RcurrentLevel, LcurrentLevel;
int freq_max;
float RsoundLevel, RsoundLevel_f;
float LsoundLevel, LsoundLevel_f;
float rainbow_steps;
int thishue = 0;
int thissat = 255;
float averK = 0.006;
int this_color;
byte HUE_START = 0;
byte HUE_STEP = 5;
String ssid = "";
String password = "";
int nowDay, nowHour, nowMinute, nowSecond;
bool IsWakeUpDay[7] = {0,0,0,0,0,0,0};
int WakeUpHourDays[7] = {0,0,0,0,0,0,0};
int WakeUpMinuteDays[7] = {0,0,0,0,0,0,0};
bool isWakeUpOn = false;

void fftRead();
void lowPass();
void volumeRead();
void spectrFFT();
void EepromGetWiFiSettings();
void EepromPutWiFiSettings();
void eeprom_upd();
void eeprom_load();
void animation();
void fillStaticColor(byte hueT_, byte satT_, byte brght);
void strobeColor(byte hueT_, byte satT_, byte brght, int per, byte repeats);
void wifiConnect();
void otaInit();
void WiFiRead();
void APactivate();
String StructDebug();
String GetData();
String CutPASS(String passNew);
String CutSSID(String ssidNew);
int cutString(String inAdd);
float cutStringFloat(String inAdd);
void FadeOff();
void FadeOn();
void BrightFade(byte newBright, bool dontAnimate = false);
bool IntervalTimer(unsigned long uptime, unsigned long *TimeOutLast, long TimeOut);
bool TimeoutTimer(unsigned long uptime, unsigned long *TimeOutLast, long *TimeOut);
bool getUTC();
void SleepTick();
void startSleep();
/*
void WakeUpTick();
void descryptWakeUp(String data);
void wakeUpProcess(bool IsChange, int DayStart, int HourStart, int MinuteStart);
void wakeUpFinish(unsigned long finishLastMillis);
*/

void setup() {
    pinMode(SOUND_R_FREQ, INPUT);
    pinMode(SOUND_R, INPUT);
    EEPROM.begin(250);
    Serial.begin(115200);
    FastLED.addLeds<STRIP_CHIP, STRIP_PIN, STRIP_COLOR>(leds, NUM_LEDS);
    FastLED.setBrightness(Bright);

    if (EEPROM.read(200) != 1) eeprom_upd();
    else if (RESET_SETTINGS == true) eeprom_upd();
    else eeprom_load();

    wifiConnect();
    lowPass();
}

void loop() {
  IfWifiDisconnect();
  ArduinoOTA.handle();
  WiFiRead();
  SleepTick();
  /*
  getUTC();
  if (isWakeUpOn) {
    wakeUpProcess(false,0,0,0);
    return;
  }
  else WakeUpTick();
  */

  if (!OnOff) {FastLED.setBrightness(0); FastLED.show(); return;}

  if (Mode == 0) volumeRead();
  else if (Mode == 1 || Mode == 2 || Mode == 3) fftRead();
  else animation();
}

void IfWifiDisconnect() {
  if (WiFi.status() != WL_CONNECTED) 
    ESP.restart();
}
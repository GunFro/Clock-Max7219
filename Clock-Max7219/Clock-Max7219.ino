#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TinyGPS++.h>
#include <Time.h>
#include <Timezone.h>
#include <SPI.h>
#include <OneButton.h>
#include <EEPROM.h>
#include "fontA.h"
#include "fontB.h"
#include "fontC.h"
#include "fontD.h"

//https://www.timeanddate.com/time/map/

TimeChangeRule _GMT = {"GMT", Last, Sun, Oct, 3, 0};       //Standard time = UTC + 0 hours
TimeChangeRule _BST = {"BST", Last, Sun, Mar, 2, 60};      //Daylight time = UTC + 1 hours
Timezone GMT(_GMT, _BST);
TimeChangeRule _CET  = {"CET",  Last, Sun, Oct, 3, 60};    //Standard time = UTC + 1 hours
TimeChangeRule _CEST = {"CEST", Last, Sun, Mar, 2, 120};   //Daylight time = UTC + 2 hours
Timezone CET(_CET, _CEST);
TimeChangeRule _UTC0 = {"UTC0", Last, Sun, Mar, 1, 0};     // UTC
Timezone UTC0(_UTC0);
TimeChangeRule _UTC1 = {"UTC1", Last, Sun, Mar, 1, 60};    // UTC +1
Timezone UTC1(_UTC1);
TimeChangeRule _UTC2 = {"UTC2", Last, Sun, Mar, 1, 120};   // UTC +2
Timezone UTC2(_UTC2);

const char string_0[] PROGMEM = "GMT-BST";
const char string_1[] PROGMEM = "CET-CEST";
const char string_2[] PROGMEM = "UTC+0";
const char string_3[] PROGMEM = "UTC+1";
const char string_4[] PROGMEM = "UTC+2";
const char* const tz_string_name[] PROGMEM = {string_0, string_1, string_2, string_3, string_4};

Timezone* timezones[] = { &GMT, &CET, &UTC0, &UTC1, &UTC2};
Timezone* TZ;                  //pointer to the time zone

char buffer[12];

OneButton button1(A2, true); // A2+A3
OneButton button2(A4, true); // A4+A5

TinyGPSPlus gps;
Max72xxPanel matrix = Max72xxPanel(10, 8, 3); // 8x3 CS, HOR, VERT

const GFXfont *font[] = {&fontA11pt7b, &fontB11pt7b, &fontC11pt7b, &fontA11Boldpt7b};
uint8_t showAll = false;
uint8_t ShowTZ = false;

// Saved in EEPROM
uint8_t tzIndex;            // indexes the timezones[] array
uint8_t fontSel;            // indexes the font[] array
uint8_t posSecOver = true;

/**********************************************************************/
void setup() {
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SS, OUTPUT);
  digitalWrite(SCK, HIGH);
  digitalWrite(MOSI, HIGH);
  digitalWrite(SS, HIGH);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
  for (byte n = 0; n <= 23; n++) {
    matrix.setRotation(n, 1); // Each display is positioned sideways
  }
  matrix.fillScreen(LOW);
  matrix.setCursor(0, 0);
  matrix.setTextWrap(false);
  matrix.setFont(NULL);
  matrix.setIntensity(0);
  matrix.write();
  matrixInit();
  dispTest();
  Serial.begin(115200);
  analogReference(DEFAULT);
  pinMode(A3, OUTPUT);   // button attached to A2 & A3.
  digitalWrite(A3, LOW); // this is 'GND'
  pinMode(A6, OUTPUT);   // button LDR to A7 & A6.
  digitalWrite(A6, LOW); // this is 'GND'
  button1.attachClick(button1Click);
  button1.attachDoubleClick(button1DoubleClick);
  button1.attachLongPressStart(button1LongPressStart);
  button1.attachLongPressStop(button1LongPressStop);
  button1.setDebounceTicks(20); // 50
  button1.setClickTicks(300); //600
  button1.setPressTicks(1000); // 1000
  pinMode(A5, OUTPUT);   // button attached to A4 & A5.
  digitalWrite(A5, LOW); // this is 'GND'
  button2.attachDoubleClick(button2DoubleClick);
  button2.attachLongPressStart(button2LongPressStart);
  button2.attachLongPressStop(button2LongPressStop);
  button2.setDebounceTicks(20); // 50
  button2.setClickTicks(300); //600
  button2.setPressTicks(1000); // 1000
  // Init EEPROM
  if (EEPROM[0] != 0x55 and EEPROM[1] != 0xAA) {
    EEPROM[0] = 0x55;
    EEPROM[1] = 0xAA;
    EEPROM[2] = 1;    // tzIndex
    EEPROM[3] = 3;    // fontSel
    EEPROM[4] = true; // posSecOver
  }
  tzIndex = EEPROM[2];
  fontSel = EEPROM[3];
  posSecOver = EEPROM[4];
  TZ = timezones[tzIndex];
}
/**********************************************************************/
void loop() {
  static byte prevSec = -1;
  byte currSec = second();
  time_t t = now();
  char sp[16];

  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }
  if (timeStatus() != timeSet) {
    matrix.drawPixel(0, 0, second() % 2);
    if (gps.time.isValid() || gps.date.isValid()) {
      setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
    }
    matrix.write();
  } else {
    if (currSec != prevSec) {
      prevSec = currSec;
      int currLight = avgIntensity();
      matrix.setIntensity(currLight);
      if (gps.time.isValid() || gps.date.isValid()) {
        setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
      }
      matrix.fillScreen(LOW);
      if (showAll) {
        matrix.setFont(NULL);
        matrix.setCursor(2, 0);
        sprintf(sp, "%2d:%02d:%02d", hour(TZ->toLocal(t)), minute(TZ->toLocal(t)), second(TZ->toLocal(t)));
        matrix.print(sp);
        matrix.setCursor(2, 9);
        sprintf(sp, "%02d-%02d-%02d", year(TZ->toLocal(t)) - 2000, month(TZ->toLocal(t)), day(TZ->toLocal(t)));
        matrix.print(sp);
        matrix.setCursor(47, 9);
        sprintf(sp, "%3i", currLight);
        matrix.print(sp);
        if (gps.satellites.isValid()) {
          matrix.setCursor(47, 0);
          sprintf(sp, "%3lu", gps.satellites.value());
          matrix.print(sp);
        }
      } else if (ShowTZ) {
        matrix.setFont(NULL);
        matrix.setCursor(2, 2);
        strcpy_P(buffer, (char*)pgm_read_word(&(tz_string_name[tzIndex])));
        sprintf(sp, "%s", buffer);
        matrix.print(sp);
        matrix.setCursor(2, 10);
        TimeChangeRule* tcr;    //pointer to current time change rule, used to get TZ abbrev
        (*TZ).toLocal(t, &tcr);
        sprintf(sp, "%s", tcr -> abbrev);
        matrix.print(sp);
      } else {
        matrix.setFont(font[fontSel]);
        if (posSecOver) {
          matrix.drawPixel(currSec + 2, 0,  HIGH);
          matrix.setCursor(1, 23);
        } else {
          matrix.drawPixel(currSec + 2, 23, HIGH);
          matrix.setCursor(1, 21);
        }
        //sprintf(sp, "%2d:%02d", hour(TZ->toLocal(t)), minute(TZ->toLocal(t)));
        sprintf(sp, "%2d:%02d", second(TZ->toLocal(t)), second(TZ->toLocal(t)));
        matrix.print(sp);
      }
      matrix.write();
      matrixInit();
    }
  }
  button1.tick();
  button2.tick();
}
/**********************************************************************/
void matrixInit() {
  // Init max7219 chips
  matrix.spiTransfer(15, 0); // OP_DISPLAYTEST
  matrix.spiTransfer(11, 7); // OP_SCANLIMIT
  matrix.spiTransfer(9, 0);  // OP_DECODEMODE
  matrix.shutdown(false);
}
/**********************************************************************/
int avgIntensity (void) {
  static int prevLight[10];
  static byte posLight;
  int currLight;

  prevLight[posLight] = 1024 - analogRead(A7);
  posLight < 9 ? posLight++ : posLight = 0;
  currLight = 0;
  for (byte ns = 0; ns < 10; ns++) {
    currLight += prevLight[ns];
  }
  currLight /= 10;
  currLight = map(currLight, 0, 1023, 0, 7);
  return currLight;
}
/**********************************************************************/
/**********************************************************************/
void button2LongPressStart() {
  // Show Timezone
  ShowTZ = true;
}
/**********************************************************************/
void button2LongPressStop() {
  ShowTZ = false;
}
/**********************************************************************/
void button2DoubleClick() {
  // Next timezone
  if ( ++tzIndex >= sizeof(timezones) / sizeof(timezones[0]) ) {
    tzIndex = 0;
  }
  EEPROM[2] = tzIndex;
  TZ = timezones[tzIndex];
}
/**********************************************************************/
/**********************************************************************/
void button1Click() {
  // Second dot over or under
  posSecOver = !posSecOver;
  EEPROM[4] = posSecOver;
}
/**********************************************************************/
void button1DoubleClick() {
  // Select a font
  if (++fontSel >= sizeof(font) / sizeof(font[0])) {
    fontSel = 0;
  }
  EEPROM[3] = fontSel;
}
/**********************************************************************/
void button1LongPressStart() {
  // Show all info, time, date, intensity and sats.
  showAll = true;
}
/**********************************************************************/
void button1LongPressStop() {
  showAll = false;
}
/**********************************************************************/
/**********************************************************************/
void dispTest() {
  matrix.setIntensity(0);
  for (int i = 0; i < 64; i++) {
    matrix.drawLine(i, 0, i, 23, HIGH);
    matrix.write();
    matrixInit();
    delay(10);
  }
  matrixInit();
  matrix.setIntensity(15);
  delay(2500);
  for (int i = 63; i >= 0; i--) {
    matrix.drawLine(i, 0, i, 23, LOW);
    matrix.write();
    matrixInit();
    delay(10);
  }
}

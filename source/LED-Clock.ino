#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TinyGPS++.h>
#include <Time.h>
#include <Timezone.h>
#include <SPI.h>
#include <OneButton.h>
#include "fontB11b7.h"
#include "fontC11b7.h"


//https://www.timeanddate.com/time/map/

TimeChangeRule _GMT = {"GMT", Last, Sun, Oct, 3, 0};           //Standard time = UTC + 0 hours
TimeChangeRule _BST = {"BST", Last, Sun, Mar, 2, 60};          //Daylight time = UTC + 1 hours
Timezone GMT(_GMT, _BST);
TimeChangeRule _CET  = {"CET",  Last, Sun, Oct, 3, 60};        //Standard time = UTC + 1 hours
TimeChangeRule _CEST = {"CEST", Last, Sun, Mar, 2, 120};       //Daylight time = UTC + 2 hours
Timezone CET(_CET, _CEST);

Timezone* timezones[] = { &GMT, &CET};
Timezone* TZ;                  //pointer to the time zone
uint8_t tzIndex = 1;            //indexes the timezones[] array
TimeChangeRule* tcr;          //pointer to the time change rule, use to get TZ abbrev

OneButton button1(7, true);
OneButton button2(6, true);

TinyGPSPlus gps;
Max72xxPanel matrix = Max72xxPanel(10, 8, 2); // 8x3 CS, HOR, VERT

const GFXfont *font[] = {&fontB11pt7b, &fontC11pt7b};
byte fontSel = 0;
byte posSecOver = true;
byte showAll = false;
byte ShowTZ = false;

/**********************************************************************/
void setup() {
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SS, OUTPUT);
  digitalWrite(SCK, HIGH);
  digitalWrite(MOSI, HIGH);
  digitalWrite(SS, HIGH);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
  for (byte n = 0; n <= 15; n++) {
    matrix.setRotation(n, 1); // Each display is positioned sideways
  }
  matrix.fillScreen(LOW);
  matrix.setCursor(0, 0);
  matrix.setTextWrap(false);
  matrix.setFont(NULL);
  matrix.setIntensity(0);
  matrix.write();
  matrixInit();
  //dispTest();
  Serial.begin(9600);
  analogReference(DEFAULT);
  pinMode(8, OUTPUT);   // button attached to 7 & 8.
  digitalWrite(8, LOW); // this is 'GND'
  button1.attachClick(button1Click);
  button1.attachDoubleClick(button1DoubleClick);
  button1.attachLongPressStart(button1LongPressStart);
  button1.attachLongPressStop(button1LongPressStop);
  button1.setDebounceTicks(50); // 50
  button1.setClickTicks(300); //600
  button1.setPressTicks(1000); // 1000
  pinMode(5, OUTPUT);   // button attached to 5 & 6.
  digitalWrite(5, LOW); // this is 'GND'
  button2.attachDoubleClick(button2DoubleClick);
  button2.attachLongPressStart(button2LongPressStart);
  button2.attachLongPressStop(button2LongPressStop);
  button2.setDebounceTicks(50); // 50
  button2.setClickTicks(300); //600
  button2.setPressTicks(1000); // 1000
  TZ = timezones[tzIndex];
}
/**********************************************************************/
void loop() {
  static byte prevSec = -1;
  byte currSec = second();
  char sp[10];

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
        sprintf(sp, "%2d:%02d:%02d", hour(TZ->toLocal(now())), minute(TZ->toLocal(now())), second(TZ->toLocal(now())));
        matrix.print(sp);
        matrix.setCursor(2, 9);
        sprintf(sp, "%02d-%02d-%02d", year(TZ->toLocal(now())) - 2000, month(TZ->toLocal(now())), day(TZ->toLocal(now())));
        matrix.print(sp);
        matrix.setCursor(47, 9);
        sprintf(sp, "%3i", currLight);
        matrix.print(sp);
        if (gps.satellites.isValid()) {
          matrix.setCursor(47, 0);
          sprintf(sp, "%3lu", gps.satellites.value());
          matrix.print(sp);
        }
      } else {
        matrix.setFont(font[fontSel]);
        if (posSecOver) {
          matrix.drawPixel(currSec + 2, 0,  HIGH);
          matrix.setCursor(5, 19);
        } else {
          matrix.drawPixel(currSec + 2, 15, HIGH);
          matrix.setCursor(5, 17);
        }
        sprintf(sp, "%2d:%02d", hour(TZ->toLocal(now())), minute(TZ->toLocal(now())));
        //sprintf(sp, "%2d:%02d", minute(TZ.toLocal(now())), second(TZ.toLocal(now())));
        matrix.print(sp);
      }
      matrix.write();
      matrixInit();
    }
  }
  button1.tick();
}
/**********************************************************************/
void matrixInit() {
  // Init max7219 chips
  matrix.spiTransfer(15, 0); //OP_DISPLAYTEST
  matrix.spiTransfer(11, 7); //OP_SCANLIMIT
  matrix.spiTransfer(9, 0);  //OP_DECODEMODE
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
  TZ = timezones[tzIndex];
}
/**********************************************************************/
/**********************************************************************/
void button1Click() {
  // Second dot over or under
  posSecOver = !posSecOver;
}
/**********************************************************************/
void button1DoubleClick() {
  // Select a font
  if (++fontSel >= sizeof(font) / sizeof(font[0])) {
    fontSel = 0;
  }
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
    //matrix.fillScreen(LOW);
    matrix.drawLine(i, 0, i, 23, HIGH);
    matrix.write();
    matrixInit();
    delay(10);
  }
  delay(500);
  for (int i = 63; i >= 0; i--) {
    //matrix.fillScreen(LOW);
    matrix.drawLine(i, 0, i, 15, LOW);
    matrix.write();
    matrixInit();
    delay(10);
  }
}

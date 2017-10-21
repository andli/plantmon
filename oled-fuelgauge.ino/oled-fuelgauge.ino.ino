#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <LiFuelGauge.h>

void lowPower() {

}

// LiFuelGauge constructor parameters
// 1. IC type, MAX17043 or MAX17044
// 2. Number of interrupt to which the alert pin is associated (Optional)
// 3. ISR to call when an alert interrupt is generated (Optional)
//
// Creates a LiFuelGauge instance for the MAX17043 IC
// and attaches lowPower to INT0 (PIN2 on most boards, PIN3 on Leonardo)
LiFuelGauge gauge(MAX17043, 0, lowPower);

// A flag to indicate a generated alert interrupt
// volatile boolean alert = false;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

unsigned long lastPushTime;
unsigned long lastMeasureTime;
const int SCREEN_HEIGHT = 32;
const int SCREEN_WIDTH = 128;
const int NUM_MEASUREMENTS = 12;
const long LOOP_INTERVAL = 250L;
const long READ_INTERVAL = 1000L * 6; //0L * 5L / 120L; //TODO: remove 12L here
const long DISPLAY_TIMEOUT = 1000L * 15L;

const int analogInPin = A0;
const int transistorMeasurePin = 4;
const int pushbuttonPin = 3;
int analogInValue = 0;
bool renderScreen = true;

int y[NUM_MEASUREMENTS];

void setup(void) {
  Serial.begin(9600);
  
  pinMode(pushbuttonPin, INPUT);    
  pinMode(transistorMeasurePin, OUTPUT);
  
  lastPushTime = 0L;
  lastMeasureTime = 0L;
  
  u8g2.begin();

  u8g2.setFont(u8g2_font_profont11_tn);

  gauge.reset();  // Resets MAX17043
  delay(200);  // Waits for the initial measurements to be made
}

void drawMeasurements(int plotX, int plotY, int plotWidth, int plotHeight) {
  const int BAR_WIDTH = (int)(plotWidth / NUM_MEASUREMENTS);

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    if (y[i] != -1) {
      u8g2.drawBox(
        plotX + (NUM_MEASUREMENTS - i - 1) * BAR_WIDTH,
        plotHeight - y[i],
        BAR_WIDTH - 2,
        plotHeight
      );
    } else {
      break;
    }
  }
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
 
void drawBattery(int screenHeight, int batteryWidth, int batteryLevelHeight) {
  u8g2.drawBox(3, 0, 4, 2); //Battery knob
  u8g2.drawFrame(0, 2, batteryWidth, screenHeight - 2); //Battery frame
  int batteryLevelPixels = (int)(gauge.getSOC() / 100 * batteryLevelHeight);
  u8g2.drawBox(2, 4 + batteryLevelHeight - batteryLevelPixels, batteryWidth - 4, batteryLevelPixels);
}

void measureMoisture() {
  digitalWrite(transistorMeasurePin, HIGH);
  delay(500);
  analogInValue = analogRead(analogInPin);
  digitalWrite(transistorMeasurePin, LOW);

  for (int k = NUM_MEASUREMENTS; k >= 0; k--) {
    y[k] = y[k - 1];
  }
  y[0] = map(analogInValue, 0, 1023, 0, SCREEN_HEIGHT);
}

void loop(void) {

  if (millis() - lastPushTime > DISPLAY_TIMEOUT && renderScreen) {
    Serial.println("display timeout");
    renderScreen = false;
  }

  if (digitalRead(pushbuttonPin) == HIGH && !renderScreen) {
    Serial.println("button pushed");
    lastPushTime = millis();
    renderScreen = true;
  }

  if (millis() - lastMeasureTime > READ_INTERVAL) {
    measureMoisture();
    lastMeasureTime = millis();
  }
  
  u8g2.firstPage();
  if (renderScreen) {
    do {
      drawBattery(32, 10, 26);
      drawMeasurements(15, 0, SCREEN_WIDTH - 15, SCREEN_HEIGHT);
      char result[8]; // Buffer big enough for 7-character float
      dtostrf(gauge.getSOC(), 6, 2, result); // Leave room for too large numbers!
      //itoa(freeRam(), result, 10);
      u8g2.drawStr(12, 8, result);
      //dtostrf(gauge.getVoltage(), 6, 2, result); // Leave room for too large numbers!
      //u8g2.drawStr(12, 32, result);
    } while ( u8g2.nextPage() );
  }

  // rebuild the picture after some delay
  delay(LOOP_INTERVAL);
}

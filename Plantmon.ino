/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// define some custom "mood" characters
#include <avr/pgmspace.h>  // this lets us refer to data in program space (i.e. flash)
const byte b8[8] = { 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111 };
const byte b7[8] = { 0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111 };
const byte b6[8] = { 0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111 };
const byte b5[8] = { 0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111 };
const byte b4[8] = { 0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111 };
const byte b3[8] = { 0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111 };
const byte b2[8] = { 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111 };
const byte b1[8] = { 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111 };

const long READ_INTERVAL = 1000L * 60L * 60L;
const int NUM_READINGS = 16;
const int SENSOR_MAX = 1000;
const int SENSOR_MIN = 200;
const int PIXELS = 16;
const int COLUMNS = 16;
const int ROWS = 2;

int readings[NUM_READINGS];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); 
int sensorPin = A0;    // select the input pin for the potentiometer

void setup() {
  Serial.begin(9600); 
  
  // set up characters
  lcd.createChar(1, b1);
  lcd.createChar(2, b2);
  lcd.createChar(3, b3);
  lcd.createChar(4, b4);
  lcd.createChar(5, b5);
  lcd.createChar(6, b6);
  lcd.createChar(7, b7);
  lcd.createChar(8, b8);

  // initialize all the readings to 0:
  //for (int arrayPos = 0; arrayPos < NUM_READINGS; arrayPos++) {
  //  readings[arrayPos] = 0;
  //}

  memset(readings,0,sizeof(readings));
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  splash();
}

void splash() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PlantMon v1.0");
  lcd.setCursor(1, 1);
  lcd.print("by andli");

  delay(3000);
  lcd.clear();
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  //if (abs(analogRead(sensorPin) -  readings[readIndex-1]) > 2) {
    int sensorVal = analogRead(sensorPin);
    if (sensorVal > SENSOR_MAX) {
      sensorVal = SENSOR_MAX;
    }
    if (sensorVal < SENSOR_MIN) {
      sensorVal = SENSOR_MIN;
    }
    //printVal(sensorVal);
    readings[readIndex] = sensorVal; //random(1, 1023);
    readIndex = readIndex + 1;
    Serial.println(readings[readIndex]);
    
    // if we're at the end of the array...
    if (readIndex >= NUM_READINGS) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }

    int lcdPos = 0;
    lcd.clear();
    for (int arrayPos = readIndex; arrayPos < COLUMNS + readIndex; arrayPos++) {
      if (arrayPos >= COLUMNS) {
        printBar(readings[arrayPos - COLUMNS], lcdPos);
      }
      else {
        printBar(readings[arrayPos], lcdPos);
      }
      lcdPos = lcdPos + 1;
    }

    delay(READ_INTERVAL);        // delay in between reads for stability
  //}  
}

void printVal(int value) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("sensor:");
  lcd.setCursor(1, 1);
  lcd.print(value, DEC);
}

void printBar(int value, int column) {
  int normValue = value - SENSOR_MIN;
  int valuesPerPixel = round((SENSOR_MAX - SENSOR_MIN) / PIXELS);
  int part = PIXELS - round(normValue / valuesPerPixel);

  if (part == 0 || normValue <= 0) {
    lcd.setCursor(column, 1);
    lcd.write(" ");  
    lcd.setCursor(column, 0);
    lcd.write(" ");
  }
  else {
    if (part > PIXELS / ROWS) {
      lcd.setCursor(column, 1);
      lcd.write(PIXELS / ROWS);
      lcd.setCursor(column, 0);
      lcd.write(part - PIXELS / ROWS);
    }
    else {
      lcd.setCursor(column, 1);
      lcd.write(part);  
      lcd.setCursor(column, 0);
      lcd.write(" ");
    }
  }
}



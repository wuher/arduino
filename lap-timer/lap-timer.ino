// liquid start
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int switchPin = 6;
int switchState = 0;
int prevSwitchState = 0;
int reply;
// liquid end

// adafruit start
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment adafruit = Adafruit_7segment();
// adafruit end

const int GATE_PIN = 7;
const int GATE_CLEAR = 1;
const int MIN_LAPTIME = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello, Setup!");
  // pinMode(A1, INPUT_PULLUP);
  pinMode(GATE_PIN, INPUT_PULLUP);

  // liquid start
  lcd.begin(16, 2);
  pinMode(switchPin, INPUT);
  lcd.print("Ask the");
  lcd.setCursor(0, 1);
  lcd.print("Crystal Ball!");
  // liquid end

  // adafruit start
  adafruit.begin(0x70);
  //adafruit.drawColon(true);
  //adafruit.print(0, DEC);
  adafruit.setBrightness(15);
  //adafruit.drawColon(true);
  adafruit.writeDigitRaw(2, 2);
  adafruit.writeDigitRaw(0, 0b00011110);
  adafruit.writeDigitRaw(1, 0b01111001);
  adafruit.writeDigitRaw(2, 0x10);
  adafruit.writeDigitRaw(3, 0b01011110);
  adafruit.writeDigitRaw(4, 0b00000110);
  adafruit.writeDisplay();
  
}

double runtime(unsigned long starttime) {
  return (millis() - starttime) / 1000.00;
}

void loop() {
  int reading = GATE_CLEAR;
  
  // wait for first interrupt
  while (digitalRead(GATE_PIN) == GATE_CLEAR) {
    delay(10);
  }

  // wait until gate clear again
  while (digitalRead(GATE_PIN) != GATE_CLEAR) {
    delay(10);
  }

  // begin race

  // start counting time
  unsigned long starttime = millis();
  Serial.println("Begin race!  ");

  // prevent false starts
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RUN!");
  
  reading = digitalRead(GATE_PIN);

  // wait for gate interrupt
  while (digitalRead(GATE_PIN) == GATE_CLEAR) {
    double t = runtime(starttime);
    lcd.setCursor(0, 1);
    lcd.print(t);
    adafruit.print(int(t * 100), DEC);
    adafruit.drawColon(true);
    adafruit.writeDisplay();
    delay(10);
  }

  Serial.println(millis() - starttime);

  float laptime = runtime(starttime);
  Serial.print("Lap time: ");
  Serial.print(laptime);
  Serial.println("s.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.setCursor(0, 1);
  lcd.print(laptime);

  delay(1000);
}

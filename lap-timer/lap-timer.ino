// == rf24 start
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
// == rf24 end

// == adafruit start
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment adafruit = Adafruit_7segment();
// == adafruit end

const int GATE_PIN = 7;
const int GATE_CLEAR = 1;
const int MIN_LAPTIME = 1;

void setup() {
  Serial.begin(9600);
  pinMode(GATE_PIN, INPUT_PULLUP);

  // == adafruit start
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
  // == adafruit end

  // == rf24 start
  radio.begin();
  // set the address
  radio.openWritingPipe(address);
  // set module as transmitter
  radio.stopListening();
  // == rf24 end
  
  Serial.println("setup done");
}

double runtime(unsigned long starttime) {
  return (millis() - starttime) / 1000.00;
}

void loop() {

  // == rf24 start

  const char text[] = "Hello World";
  Serial.print("transmitting... ");
  Serial.println(radio.write(&text, sizeof(text)));
  Serial.print(text);
  Serial.println(" transmitted");

  // == rf24 end
  
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
  
  reading = digitalRead(GATE_PIN);

  // wait for gate interrupt
  while (digitalRead(GATE_PIN) == GATE_CLEAR) {
    double t = runtime(starttime);
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

  delay(1000);
}

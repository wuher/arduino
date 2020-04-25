/*
 * File name:    lap-timer.ino
 * File Created: 2019-01-19T21:00:34
 *
 * Arduino lap timer that supports second gate wirelessly.
 *
 * State diagram:
 *
 *    https://www.lucidchart.com/invitations/accept/1b5df8e3-d3ed-4f34-a198-4cc613daef91
 */

#include "timertypes.h"

// == RF24 start
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
// == RF24 end

// == adafruit start
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment adafruit = Adafruit_7segment();
// == adafruit end

const int GATE_PIN = 7;
const int GATE_CLEAR = 1;
const int MIN_LAPTIME = 1;

/**
 * Set up adafruit display
 */
void setupAdafruit() {
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

/**
 * Set up RF24 communication as a receiver.
 */
void setupComm() {
  radio.begin();
  // set the address
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  // set module as receiver
  radio.startListening();
}

/**
 * Setup Arduino.
 */
void setup() {
  Serial.begin(9600);

  pinMode(3, OUTPUT);
  // beam sensor
  pinMode(GATE_PIN, INPUT_PULLUP);
  // display
  setupAdafruit();
  // comm
  setupComm();

  Serial.println("setup done");
}

/**
 * Calculate runtime based on the given start time.
 */
double runtime(unsigned long starttime) {
  return (millis() - starttime) / 1000.00;
}

// -- program globals -------------
State state = RACE_OVER;
unsigned long rxLastPing = millis();
unsigned long startTime = 0;
// --------------------------------

void checkPodConnection(PodMsg msg) {
  unsigned long now = millis();
  if (msg == POD_HEARTBEAT) {
    digitalWrite(3, LOW);
    rxLastPing = now;
  } else if (now - rxLastPing > 3000) {
    digitalWrite(3, HIGH);
  }
}

/**
 * Read beam senor / gate state.
 */
GateState getGateStatus() {
  return digitalRead(GATE_PIN) == 1 ? GATE_OPEN : GATE_CLOSED;
}

/**
 * Check if pod has sent any messages.
 */
PodMsg checkMessages() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));

    Serial.print("RX: ");
    Serial.println(text);

    if (strcmp(text, "interrupt") == 0) {
      return POD_GATE;
    } else if (strcmp(text, "ping") == 0) {
      return POD_HEARTBEAT;
    } else {
      return POD_NOTHING;
    }
  } else {
    return POD_NOTHING;
  }
}

/**
 * Starts the timer.
 *
 * Call this when transferring to RACE_ON state.
 */
void raceStarted() {
  startTime = millis();
  Serial.println("Begin race!");
  // prevent false starts
  delay(500);
}

/**
 * State handler for RACE_OVER state.
 */
State takeActionWhenRaceOver(GateState gatestate, PodMsg podmsg) {
  if (gatestate == GATE_CLOSED) {
    Serial.println("** RACE_OVER -> RACE_START");
    return RACE_START;
  }

  if (podmsg == POD_GATE) {
    Serial.println("** RACE_OVER -> RACE_ON");
    raceStarted();
    return RACE_ON;
  }

  return RACE_OVER;
}

/**
 * State handler for RACE_START state.
 */
State takeActionWhenRaceStart(GateState gatestate, PodMsg podmsg) {
  if (gatestate == GATE_OPEN) {
    Serial.println("** RACE_START -> RACE_ON");
    raceStarted();
    return RACE_ON;
  }

  return RACE_START;
}

/**
 * State handler for RACE_ON state.
 */
State takeActionWhenRaceOn(GateState gatestate, PodMsg podmsg) {
  if (gatestate == GATE_CLOSED || podmsg == POD_GATE) {
    Serial.println("** RACE_ON -> RACE_OVER");
    Serial.println(millis() - startTime);
    float laptime = runtime(startTime);
    Serial.print("Lap time: ");
    Serial.print(laptime);
    Serial.println("s.");
    digitalWrite(3, HIGH);
    delay(1000);
    return RACE_OVER;
  } else {
    double t = runtime(startTime);
    adafruit.print(int(t * 100), DEC);
    adafruit.drawColon(true);
    adafruit.writeDisplay();
  }

  return RACE_ON;
}

/**
 * Just calls appropriate state handler based on current state.
 */
State takeAction(State currstate, GateState gatestate, PodMsg podmsg) {
  if (currstate == RACE_OVER) {
    return takeActionWhenRaceOver(gatestate, podmsg);
  } else if (currstate == RACE_START) {
    return takeActionWhenRaceStart(gatestate, podmsg);
  } else if (currstate == RACE_ON) {
    return takeActionWhenRaceOn(gatestate, podmsg);
  } else {
    Serial.println("ERROR: unknown state");
  }
}

/**
 * The main loop.
 */
void loop() {
  // read gate state
  GateState gate = getGateStatus();
  // check messages
  PodMsg msg = checkMessages();
  // take action based on current state, gate state and pod message
  state = takeAction(state, gate, msg);
  // slight delay before next loop
  checkPodConnection(msg);
  delay(10);
}

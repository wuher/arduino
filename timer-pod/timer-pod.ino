// -- radio start --
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN
//address through which two modules communicate.
const byte address[6] = "00001";
// -- radio end --

const int GATE_PIN = 7;
const int GATE_CLEAR = 1;

double runtime(unsigned long starttime) {
  return (millis() - starttime) / 1000.00;
}

void sendPing() {
  const char text[] = "ping";
  radio.write(&text, sizeof(text));
  Serial.print(text);
  Serial.println(" transmitted");
}

void sendMessage() {
  const char text[] = "gate";
  Serial.println("transmitting...");
  radio.write(&text, sizeof(text));
  Serial.print(text);
  Serial.println(" transmitted");
}

void setup() {
  Serial.begin(9600);

  // -- sensor
  pinMode(GATE_PIN, INPUT_PULLUP);

  // -- radio
  radio.begin();
  // set the address
  radio.openWritingPipe(address);
  // set module as transmitter
  radio.stopListening();

  Serial.println("setup done");
}

unsigned long lastping = millis();
void maybeSendPing() {
  unsigned long now = millis();
  if (now - lastping > 1000) {
    sendPing();
    lastping = millis();
  }
}

void loop() {
  // wait for first interrupt
  while (digitalRead(GATE_PIN) == GATE_CLEAR) {
    maybeSendPing();
    delay(10);
  }

  // wait until gate clear again
  while (digitalRead(GATE_PIN) != GATE_CLEAR) {
    delay(10);
  }

  sendMessage();
}

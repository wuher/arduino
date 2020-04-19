const int GATE_PIN = 7;
const int GATE_CLEAR = 1;

double runtime(unsigned long starttime) {
  return (millis() - starttime) / 1000.00;
}

void setup() {
  Serial.begin(9600);
  pinMode(GATE_PIN, INPUT_PULLUP);
  Serial.println("setup done");
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
  
  reading = digitalRead(GATE_PIN);

  // wait for gate interrupt
  while (digitalRead(GATE_PIN) == GATE_CLEAR) {
    double t = runtime(starttime);
    delay(10);
  }

  Serial.println(millis() - starttime);

  float laptime = runtime(starttime);
  Serial.print("Lap time: ");
  Serial.print(laptime);
  Serial.println("s.");

  delay(1000);
}

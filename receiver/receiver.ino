//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

void setup()
{
  radio.begin();
  
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();

  Serial.println("setup done");
}

void loop()
{
  //Send message to receiver
  const char text[] = "Hello World";
  Serial.println("transmitting...");
  radio.write(&text, sizeof(text));
  Serial.print(text);
  Serial.println(" transmitted");
  delay(5000);
}

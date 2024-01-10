#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 myRadio(7, 6);
// byte addresses[][6] = {"0"};

const uint64_t address = 0XB00B1E50C3LL;

struct package {
  char text[300] = "TEST";
};

typedef struct package Package;
Package dataReceive;
Package dataTransmit;

void setup() {

  myRadio.begin();

  myRadio.setRetries(15,15);
  myRadio.setPayloadSize(32);

  myRadio.setChannel(114);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate(RF24_250KBPS);

  // myRadio.openReadingPipe(1, addresses[0]);
  myRadio.openReadingPipe(1, address);
  myRadio.startListening();

  Serial.begin(115200);
}

void loop() {

  if (myRadio.available()) {
    while (myRadio.available()) {
      myRadio.read(&dataReceive, sizeof(dataReceive));
    }
    // Serial.println("Receive: ");
    Serial.println(dataReceive.text);
    Serial.print("\n");
  }


  delay(1000); // Slower delay

  myRadio.stopListening();

  // Serial.println("Enter text to transmit:");
  char inData[300];
  int index = 0;

  if (Serial.available()) 
  {
    while (Serial.available()) 
    {
      char receivedChar = Serial.read();
      if (index < 299 && receivedChar != '\n') 
      {
        inData[index] = receivedChar;
        index++;
        inData[index] = '\0';
        sprintf(dataTransmit.text, "%s", inData);
        // myRadio.openWritingPipe(address);
        // myRadio.write(&dataTransmit, sizeof(dataTransmit));
      }
    }
  }

  if (strlen(dataTransmit.text) > 0)
  {
    // Serial.print("Transmit: ");
    // Serial.println(dataTransmit.text);
    // myRadio.openWritingPipe(addresses[0]);
    myRadio.openWritingPipe(address);
    myRadio.write(&dataTransmit, sizeof(dataTransmit));
  }


  // myRadio.openWritingPipe(addresses[0]);
  // myRadio.openWritingPipe(address);
  // myRadio.write(&dataTransmit, sizeof(dataTransmit));

  // myRadio.openReadingPipe(1, addresses[0]);
  myRadio.openReadingPipe(1, address);
  myRadio.startListening();

}

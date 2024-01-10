#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <NewPing.h>

Servo steeringServo;

NewPing sonar(10, 8);  // Trig pin = 10, Echo pin = 6

int servoPin = 9;

int motor1pin1 = 2;
int motor1pin2 = 3;
int motor2pin1 = 4;
int motor2pin2 = 5;

int distance = 0;

int stopDist = 50; 

unsigned long delayTime = 2000;
unsigned long previousTime = 0;

RF24 myRadio(6, 7);

// byte addresses[][6] = {"0"};

const uint64_t address = 0XB00B1E50C3LL;

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

  steeringServo.attach(servoPin);
  steeringServo.write(90);

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  Serial.begin(115200);

}

struct package {
char text[300] = "Text to be transmitted";
};

typedef struct package Package;
Package dataReceive;
Package dataTransmit;

void loop() {

  steeringServo.write(90);
  delay(500);
  int distance = getDistance();
  if(distance >= stopDist)
  {
    moveForward();
  }
  while(distance >= stopDist)
  {
    distance = getDistance();
    delay(250);
  }
    
  if((distance < stopDist) && (distance != 0))
  { stopMove();
  int turnDir = checkDirection();
  Serial.print(turnDir);
  switch (turnDir)
  {
    case 0: 
      turnLeft (1300);
      break;
    case 1:  
       turnLeft (1950);
       break;
    case 2: 
      turnRight (1300);
      break;
  }
  }
}

void moveForward()
{
  steeringServo.write(90);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH); 
}

void stopMove()
{
  steeringServo.write(90);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW); 
}

void turnRight(int duration)
{
  steeringServo.write(135);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW); 
  delay(duration); // Wait for the specified duration
  stopMove(); // Stop after turning
}

void turnLeft(int duration)
{
  steeringServo.write(45);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH); 
  delay(duration); // Wait for the specified duration
  stopMove(); // Stop after turning
}

int getDistance()
{
  delayMicroseconds(11);
  distance = sonar.ping_cm(); 
  
  Serial.print("DISTANCE: ");
  Serial.println(distance);
  return distance;
}

char receivedChar[300];

int checkDirection()
{

  int distances[2];
  int turnDir = -1;
  steeringServo.write(135);
  delay(1000);
  distances[0] = getDistance();
  steeringServo.write(45);
  delay(1000);
  distances[1] = getDistance();

  // int turnDir = -1;

  if (myRadio.available())
  {
    while (myRadio.available())
    {
      myRadio.read(&dataReceive, sizeof(dataReceive));
    }

    Serial.println("Receive: ");
    Serial.println(dataReceive.text);
    Serial.print("\n");

    // Check if the received string includes the word "right"
    if (strstr(dataReceive.text, "ri") != NULL)
    {
      Serial.println("TURN RIGHT!");
      turnDir = 2;
      // return turnDir;
    }
    else if (strstr(dataReceive.text, "le") != NULL)
    {
      Serial.println("TURN LEFT!");
      turnDir = 0;
      // return turnDir;
    }
    else if (strstr(dataReceive.text, "ar") != NULL)
    {
      Serial.println("TURN AROUND!");
      turnDir = 1;
      // return turnDir;
    }
  }

  delay(1000); // Slower delay

  myRadio.stopListening();

  char inData[300];

  if ((distances[0] < stopDist) && (distances[1] < stopDist))
  {
    strcpy(receivedChar, "There are walls: right and left");
    Serial.print("receivedChar: ");
    Serial.println(receivedChar);
    strcpy(inData, receivedChar);
    sprintf(dataTransmit.text, "%s", inData);
  }
  else if (distances[0] < stopDist)
  {
    strcpy(receivedChar, "There are walls: left");
    Serial.print("receivedChar: ");
    Serial.println(receivedChar);
    strcpy(inData, receivedChar);
    sprintf(dataTransmit.text, "%s", inData);
  }
  else if (distances[1] < stopDist)
  {
    strcpy(receivedChar, "There are walls: right");
    Serial.print("receivedChar: ");
    Serial.println(receivedChar);
    strcpy(inData, receivedChar);
    sprintf(dataTransmit.text, "%s", inData);
  }

  if (strlen(dataTransmit.text) > 0)
  {
    Serial.print("Transmit: ");
    Serial.println(dataTransmit.text);
    // myRadio.openWritingPipe(addresses[0]);
    myRadio.openWritingPipe(address);
    myRadio.write(&dataTransmit, sizeof(dataTransmit));
  }

  // myRadio.openReadingPipe(1, addresses[0]);
  myRadio.openReadingPipe(1, address);
  myRadio.startListening();

  return turnDir;

}

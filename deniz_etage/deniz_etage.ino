// ------------------------
// Deniz Kahriman 0984809 -
// ------------------------

#include <Wire.h>

#define DENIZ_ETAGE 2
#define ANSWERSIZE 3

// led's
int whiteLed = 5; // led up button
int redLed = 3;   // led down button
int ledPin = 7;   // led for here

// buttons
int buttonPinUp = 4;
int buttonPinDown = 2;
int buttonStateUp = 0;
int buttonStateDown = 0;

// reed
int reedSwitchPin = 8; // reed sensor pin connected to

// shiftout
#define CLOCKPIN 10  // pin connected to SRCLK of 74HC595 
#define DATAPIN 11   // pin connected to SER of 74HC595 
#define LATCHPIN 12  // pin connected to RCLK of 74HC595

// 7 segment display
const byte datArray[] = {
  B01111110,  // 0
  B00001100,  // 1
  B10110110,  // 2
  B10011110,  // 3
  B11001100,  // 4
  B11011010,  // 5
  B11111010,  // 6
  B00001110,  // 7
  B11111110,  // 8
  B11011110   // 9
};

// i2c
int liftEtage = 0;
int liftState = 0;
int liftHere = 0;
int liftStop = 0;
int liftStopAccepted = 0;

#define LIFTNOTMOVING 0
#define LIFTMOVING 1
#define LIFTWAITING 2

#define NOSTOP 0
#define STOPFORUP 1
#define STOPFORDOWN -1

int blinkTime = millis();
bool blinkState = false;

void setup() {
  // setup i2c
  Serial.begin(9600);
  Serial.println("Deniz etage online");
  Wire.begin(DENIZ_ETAGE);  //join i2c bus with address 2
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // all the lift parts
  pinMode(whiteLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
  pinMode(reedSwitchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);

  // begin with leds off
  digitalWrite(redLed, LOW);
  digitalWrite(whiteLed, LOW);
  digitalWrite(ledPin, LOW);
}

// function for 7 segment display
void writeDigit(int i) {
  digitalWrite(LATCHPIN, LOW); // latchPin low for duration of transmission
  shiftOut(DATAPIN, CLOCKPIN, MSBFIRST, datArray[i]); // send data
  digitalWrite(LATCHPIN, HIGH); // latchPin high to save the data
}

// i2c receive
void receiveEvent() {
  Serial.println("receive");
  liftEtage = Wire.read();
  liftState = Wire.read();
  if (liftState == LIFTWAITING && liftHere) {
    liftStopAccepted = 0;
  }

  int newLiftStopAccepted = Wire.read();
  if (newLiftStopAccepted != 0) {
    liftStopAccepted = newLiftStopAccepted;
  }
}

// i2c request
void requestEvent() {
  Serial.println("request");
  Wire.write(1);
  Wire.write(liftHere);
  Wire.write(liftStop);
  if (liftStop != 0) {
    liftStop = 0;
  }
}

void loop() {
  // check if lift is here
  liftHere = digitalRead(reedSwitchPin);
  if (liftState != LIFTMOVING && liftHere) {
    digitalWrite(ledPin, HIGH);
    writeDigit(liftEtage);
  } else {
    writeDigit(liftEtage);
  }

  // blink display
  if (liftState == LIFTMOVING) {
    if (blinkState == 0 && millis() - blinkTime > 100) {
      blinkState = 1;
      blinkTime = millis();
    }
    if (blinkState == 1 && millis() - blinkTime > 100) {
      blinkState = 0;
      blinkTime = millis();
    }
  }

  // button for stop up
  if (liftStopAccepted == STOPFORUP) {
    digitalWrite(whiteLed, HIGH);
  }

  buttonStateUp = digitalRead(buttonPinUp);
  if (buttonStateUp == LOW &&
    !(liftState == LIFTWAITING && liftHere) &&
    liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOPFORUP;
  }

  // button for stop down
  if (liftStopAccepted == STOPFORDOWN) {
  digitalWrite(redLed, HIGH);
  }

  buttonStateDown = digitalRead(buttonPinDown);
  if (buttonStateDown == LOW &&
    !(liftState == LIFTWAITING && liftHere) &&
    liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOPFORDOWN;
  }
}

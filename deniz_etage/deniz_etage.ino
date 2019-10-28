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
bool buttonUpPress = false;
bool buttonDownPress = false;

int buttonPinUp = 4;
int buttonPinDown = 2;
int buttonStateUp = 0;

int buttonStateDown = 0;
int lastStateUp = 0;
int lastStateDown = 0;

// reed
int reedSwitchPin = 8; // reed sensor pin connected to

// shiftout
#define clockPin 10  // pin connected to SRCLK of 74HC595 
#define dataPin 11   // pin connected to SER of 74HC595 
#define latchPin 12  // pin connected to RCLK of 74HC595

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

#define liftNotMoving 0
#define liftMoving 1
#define liftWaiting 2

#define noStop 0
#define stopForUp -1
#define stopForDown 1

// blink display
bool blinkState = false;
long blinkTime = millis();

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
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // begin with leds off
  digitalWrite(redLed, LOW);
  digitalWrite(whiteLed, LOW);
  digitalWrite(ledPin, LOW);
}

// function for 7 segment display
void writeDigit(int i) {
  digitalWrite(latchPin, LOW); // latchPin low for duration of transmission
  shiftOut(dataPin, clockPin, MSBFIRST, datArray[i]); // send data
  digitalWrite(latchPin, HIGH); // latchPin high to save the data
}

//
void receiveEvent() {
  Serial.println("receive");
  liftEtage = Wire.read();
  liftState = Wire.read();
  if (liftState == liftWaiting && liftHere) {
    liftStopAccepted = 0;
  }

  int newLiftStopAccepted = Wire.read();
  if (newLiftStopAccepted != 0) {
    liftStopAccepted = newLiftStopAccepted;
  }
}

//
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

  if (liftHere == HIGH) {
    Serial.println("Reed active");
    Serial.println("Led for here on");
    liftHere = 1;
    // writeDigit(2);
    digitalWrite(ledPin, HIGH);
    digitalWrite(whiteLed, LOW);
    digitalWrite(redLed, LOW);
  } else {
    digitalWrite(ledPin, LOW);
    writeDigit(liftEtage);
  }

  // if lift is moving blink display
  if (liftState == liftMoving) {
  }
  else if (blinkState == false) {
    if (blinkTime + 100 < millis()) {
      blinkState = true;
      blinkTime = millis();
    }
  }
  else if (blinkState == true) {
    if (blinkTime + 100 < millis()) {
      blinkState = false;
      blinkTime = millis();
    }
  }

  // button for stop up
  buttonStateUp = digitalRead(buttonPinUp);
  if (buttonStateUp != lastStateUp) {
    if (buttonStateUp == LOW) {
      buttonUpPress = true;
      liftStop = stopForUp;
      Serial.println("Up pressed");
      digitalWrite(whiteLed, HIGH);
      Serial.println("White led on");
    } else {
      Serial.println("Up not pressed");
    }
  }
  lastStateUp = buttonStateUp;

  // button for stop down
  buttonStateDown = digitalRead(buttonPinDown);
  if (buttonStateDown != lastStateDown) {
    if (buttonStateDown == LOW) {
      buttonDownPress = true;
      liftStop = stopForDown;
      Serial.println("Down pressed");
      digitalWrite(redLed, HIGH);
      Serial.println("Red led on");
    } else {
      Serial.println("Down not pressed");
    }
  }
  lastStateDown = buttonStateDown;
}

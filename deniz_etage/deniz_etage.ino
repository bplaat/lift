// ------------------------
// Deniz Kahriman 0984809 -
// ------------------------

#include <Wire.h>

#define DENIZ_ETAGE 3
#define ANSWERSIZE 3

// led's
#define WHITELED 5 // led up button
#define REDLED 3   // led down button
#define LEDPIN 7   // led for here

// buttons
#define BUTTONPINUP 4
#define BUTTONPINDOWN 2

int buttonStateUp = 0;
int buttonStateDown = 0;

// reed
#define REEDSWITCHPIN 8 // reed sensor pin connected to

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
  Wire.begin(DENIZ_ETAGE);  //join i2c bus with address 2
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // all the lift parts
  pinMode(WHITELED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(BUTTONPINUP, INPUT_PULLUP);
  pinMode(BUTTONPINDOWN, INPUT_PULLUP);
  pinMode(REEDSWITCHPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);

  // begin with leds off
  digitalWrite(REDLED, LOW);
  digitalWrite(WHITELED, LOW);
  digitalWrite(LEDPIN, LOW);
}

void clearDigit() {
  digitalWrite(LATCHPIN, LOW);
}

// function for 7 segment display
void writeDigit(int i) {
  digitalWrite(LATCHPIN, LOW); // latchPin low for duration of transmission
  shiftOut(DATAPIN, CLOCKPIN, MSBFIRST, datArray[i]); // send data
  digitalWrite(LATCHPIN, HIGH); // latchPin high to save the data
}

// i2c receive
void receiveEvent() {
  liftEtage = Wire.read();
  liftState = Wire.read();

  int newLiftStopAccepted = Wire.read();
  if (newLiftStopAccepted != 0) {
    liftStopAccepted = newLiftStopAccepted;
  }
}

// i2c request
void requestEvent() {
  Wire.write(1);
  Wire.write(liftHere);
  Wire.write(liftStop);
  if (liftStop != 0) {
    liftStop = 0;
  }
}

void loop() {
  // check if lift is here
  liftHere = digitalRead(REEDSWITCHPIN);
  if (liftState != LIFTMOVING && liftHere) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }

  if (liftState == LIFTWAITING && liftHere) {
    liftStopAccepted = 0;
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

    if (blinkState == 1) {
      writeDigit(liftEtage);
    } else {
      clearDigit();
    }
  } else {
    writeDigit(liftEtage);
  }

  // button for stop up
  if (liftStopAccepted == STOPFORUP) {
    digitalWrite(WHITELED, HIGH);
  } else {
    digitalWrite(WHITELED, LOW);
  }

  buttonStateUp = digitalRead(BUTTONPINUP);
  if (buttonStateUp == LOW &&
    !(liftState == LIFTWAITING && liftHere) &&
    liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOPFORUP;
  }

  // button for stop down
  if (liftStopAccepted == STOPFORDOWN) {
    digitalWrite(REDLED, HIGH);
  } else {
    digitalWrite(REDLED, LOW);
  }

  buttonStateDown = digitalRead(BUTTONPINDOWN);
  if (buttonStateDown == LOW &&
    !(liftState == LIFTWAITING && liftHere) &&
    liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOPFORDOWN;
  }
}

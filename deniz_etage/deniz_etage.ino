// ------------------------
// Deniz Kahriman 0984809 -
// ------------------------

#include <Wire.h>

#define DENIZ_ETAGE 3
#define ANSWERSIZE 3
#define ETAGE_OFFSET 10

// led's
#define WHITELED 5 // led up button
#define REDLED 3   // led down button
#define LEDPIN 7   // led for here

// buttons
#define BUTTON_PIN_UP 4
#define BUTTON_PIN_DOWN 2

int buttonStateUp = 0;
int buttonStateDown = 0;

// reed
#define REED_SWITCH_PIN 8 // reed sensor pin connected to

// shiftout
#define CLOCK_PIN 10  // pin connected to SRCLK of 74HC595 
#define DATA_PIN 11   // pin connected to SER of 74HC595 
#define LATCH_PIN 12  // pin connected to RCLK of 74HC595

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

#define LIFT_NOT_MOVING 0
#define LIFT_MOVING 1
#define LIFT_WAITING 2

#define NO_STOP 0
#define STOP_FOR_UP 1
#define STOP_FOR_DOWN -1

unsigned long blinkTime = millis();
bool blinkState = false;

void setup() {
  // setup i2c
  Wire.begin(DENIZ_ETAGE + ETAGE_OFFSET);  //join i2c bus with address 2
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // all the lift parts
  pinMode(WHITELED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(BUTTON_PIN_UP, INPUT_PULLUP);
  pinMode(BUTTON_PIN_DOWN, INPUT_PULLUP);
  pinMode(REED_SWITCH_PIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // begin with leds off
  pinMode(REDLED, LOW);
  pinMode(WHITELED, LOW);
  pinMode(LEDPIN, LOW);
}

void clearDigit() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0); // send data
  digitalWrite(LATCH_PIN, HIGH); // LATCH_PIN high to save the data
}

// function for 7 segment display
void writeDigit(int i) {
  digitalWrite(LATCH_PIN, LOW); // LATCH_PIN low for duration of transmission
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, datArray[i]); // send data
  digitalWrite(LATCH_PIN, HIGH); // LATCH_PIN high to save the data
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
  liftHere = digitalRead(REED_SWITCH_PIN);
  if (liftState != LIFT_MOVING && liftHere) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }

  if (liftState == LIFT_WAITING && liftHere) {
    liftStopAccepted = 0;
  }

  // blink display
  if (liftState == LIFT_MOVING) {
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
  if (liftStopAccepted == STOP_FOR_UP) {
    digitalWrite(WHITELED, HIGH);
  } else {
    digitalWrite(WHITELED, LOW);
  }

  buttonStateUp = digitalRead(BUTTON_PIN_UP);
  if (buttonStateUp == LOW &&
      !(liftState == LIFT_WAITING && liftHere) &&
      liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOP_FOR_UP;
  }

  // button for stop down
  if (liftStopAccepted == STOP_FOR_DOWN) {
    digitalWrite(REDLED, HIGH);
  } else {
    digitalWrite(REDLED, LOW);
  }

  buttonStateDown = digitalRead(BUTTON_PIN_DOWN);
  if (buttonStateDown == LOW &&
      !(liftState == LIFT_WAITING && liftHere) &&
      liftStopAccepted == 0 && liftStop == 0) {
    liftStop = STOP_FOR_DOWN;
  }
}

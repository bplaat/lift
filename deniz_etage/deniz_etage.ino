// Deniz Kahriman 0984809
// ----------------------

#include <Wire.h>
#include <stdint.h>
#include <Arduino.h>

#define DENIZ_ETAGE 2
#define stopForDown -1
#define noStop 0
#define stopForUp 1

int liftSendStop = 0;
int liftHere = 0;

// led's
int whiteLed = 6; // led up button
int redLed = 4;  // led down button
int ledPin = 7; // led here

// buttons
bool buttonUpPress = false;    // button up input
bool buttonDownPress = false; // button down input
int buttonPinUp = 5;
int buttonPinDown = 3;
int buttonStateUp = 0;
int buttonStateDown = 0;
int lastStateUp = 0;
int lastStateDown = 0;

// reed
int reedSwitchPin = 8; // reed sensor pin connected to


// shiftout
#define clockPin 10   // pin connected to SRCLK of 74HC595 
#define dataPin 11   // pin connected to SER of 74HC595 
#define latchPin 12 // pin connected to RCLK of 74HC595

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
byte receiveEtage = 0;
byte receiveDirection = 0;
byte receiveStop = 0;
byte sendStop = 0;


void setup() {

  Serial.begin(9600);

  Wire.begin(DENIZ_ETAGE);  //join i2c bus with address 2
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.println("Deniz etage online");

  pinMode(whiteLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
  pinMode(reedSwitchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(redLed, LOW);
  digitalWrite(whiteLed, LOW);
  digitalWrite(ledPin, LOW);

}

void writeDigit(int i) {
  digitalWrite(latchPin, LOW); // latchPin low for duration of transmission
  shiftOut(dataPin, clockPin, MSBFIRST, datArray[i]); // send data
  digitalWrite(latchPin, HIGH); // latchPin high to save the data
}


void loop() {

  buttonStateUp = digitalRead(buttonPinUp);

  if (buttonStateUp != lastStateUp) {
    if (buttonStateUp == LOW) {
      buttonUpPress = true;
      liftSendStop = stopForUp;
      Serial.println("Up");
      digitalWrite(whiteLed, HIGH);
      Serial.println("White led on");
    } else {
      Serial.println("white led off");
    }
    delay(50);
  }
  lastStateUp = buttonStateUp;



  buttonStateDown = digitalRead(buttonPinDown);

  if (buttonStateDown != lastStateDown) {
    if (buttonStateDown == LOW) {
      buttonDownPress = true;
      liftSendStop = stopForDown;
      Serial.println("Down");
      Serial.println("Red led on");
      digitalWrite(redLed, HIGH);
    } else {
      Serial.println("Red led off");
    }
    delay(50);
  }
  lastStateDown = buttonStateDown;
  



  liftHere = digitalRead(reedSwitchPin);

  if (liftHere == HIGH) {
    Serial.println("Reed active");
    Serial.println("Led for here on");
    writeDigit(2);
    digitalWrite(ledPin, HIGH);
    digitalWrite(whiteLed, LOW);
    digitalWrite(redLed, LOW);
  } else {
    digitalWrite(ledPin, LOW);
    writeDigit(0);
  }


  
}

void receiveEvent() {

  Serial.println("receive");
  receiveEtage = Wire.read();
  receiveDirection = Wire.read();
  receiveStop = Wire.read();

}

void requestEvent() {

  Serial.println("request");
  Wire.write(1);
  Wire.write(liftHere);
  Wire.write(sendStop);

}

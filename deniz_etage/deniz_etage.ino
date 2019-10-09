 // Deniz Kahriman 0984809
 
 #include "digit_display.h"
 #include <Wire.h>
 #include <stdint.h>
 #include <Arduino.h>

 #define DENIZ_ETAGE 2
 
 #define stopForDown -1
 #define noStop 0
 #define stopForUp 1
 
 int liftSendStop = 0;

 int whiteLed = A2; // led up button
 int redLed = A4;  // led down button
 int ledPin = 9;  // led here 
 
 bool buttonUpPress = false;    // button up input
 bool buttonDownPress = false; // button down input
 const int buttonPinUp = A3;
 const int buttonPinDown = A5;

 int reedSwitchPin = 10; // reed sensor pin connected to
 int liftHere = 0;
 
 // i2c
 byte receiveEtage = 0;
 byte receiveDirection = 0;
 byte receiveStop = 0;
 byte sendStop = 0;

 // 7 segment display
 DigitDisplay *display;
 uint8_t display_pins[7] = {2, 3, 4, 5, 6, 7, 8};
 int displayMaster = 0;

 
void setup() { 

 Serial.begin(9600);
 
 Wire.begin(DENIZ_ETAGE);  //join i2c bus with address 2
 Serial.println("Deniz etage online");
 Wire.onReceive(receiveEvent);          
 Wire.onRequest(requestEvent);
 
 pinMode(whiteLed, OUTPUT);
 pinMode(redLed, OUTPUT);
 pinMode(buttonPinUp, INPUT_PULLUP);
 pinMode(buttonPinDown, INPUT_PULLUP);
 pinMode(reedSwitchPin, INPUT);
 pinMode(ledPin, OUTPUT);
 
 display = digit_display_new(display_pins);
 
 digitalWrite(redLed, LOW);
 digitalWrite(whiteLed, LOW); 
 
}

void loop() {

 digit_display_set_digit(display,displayMaster);


 if (digitalRead(buttonPinUp)){
   liftSendStop = stopForUp;
   
 }else if (digitalRead(buttonPinUp)){
  liftSendStop = stopForDown;
  
 }else{
  liftSendStop = noStop;
 }


 if (liftSendStop == LOW){
      buttonUpPress = true;   
      Serial.println("White led on");
      digitalWrite(whiteLed, HIGH);
    } else {
      Serial.println("off");
    }
     
 if (liftSendStop == LOW){
      buttonDownPress = true;
      Serial.println("Red led on");
      digitalWrite(redLed, HIGH);
    } else {
     Serial.println("off");
    }
  

 liftHere = digitalRead(reedSwitchPin);
 
  if (liftHere == HIGH) {
    Serial.println("Reed active");
    Serial.println("Led for here on");
    digitalWrite(ledPin, HIGH);
    digitalWrite(whiteLed, LOW);
    digitalWrite(redLed, LOW);
  }else{
    digitalWrite(ledPin, LOW);
  }
 }

void receiveEvent(){

 Serial.println("receive");
 receiveEtage = Wire.read();
 receiveDirection = Wire.read();
 receiveStop = Wire.read();
 displayMaster = Wire.read();
                  
}

void requestEvent(){

  Serial.println("request");  
  Wire.write(1);
  Wire.write(liftHere);
  Wire.write(sendStop);
    
}

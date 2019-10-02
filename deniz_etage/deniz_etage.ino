 #include <Wire.h>
 #include <stdint.h>
 #include <Arduino.h>

 const int a = 7;  //segment "a"
 const int b = 8;  //segment "b"
 const int c = 4;  //segment "c"
 const int d = 5;  //segment "d"
 const int e = 6;  //segment "e"
 const int f = 2;  //segment "f"
 const int g = 3;  //segment "g"


 #define whiteLed 10
 #define redLed 13
 #define DENIZ_ETAGE 2
 #define noStopNeeded 0
 #define stopForUp 1
 #define stopForDown 2

 bool buttonUpPress = false;   
 bool buttonDownPress = false; 

 const int buttonPinUp = 11;
 const int buttonPinDown = 12;

 int reedSwitchPin = A5;
 int stateReed = 0;
 int liftHere = 0;
 int ledPin = 9;
 
 byte masterEtage = 0;
 byte masterDirection = 0;
 byte masterStop = 0;


void setup() {  
  
 pinMode(a, OUTPUT);
 pinMode(b, OUTPUT);
 pinMode(c, OUTPUT);
 pinMode(d, OUTPUT);
 pinMode(e, OUTPUT);
 pinMode(f, OUTPUT);
 pinMode(g, OUTPUT);

 Serial.print(9600);
 Wire.begin(DENIZ_ETAGE);
 Wire.onReceive(receiveEvent);          
 Wire.onRequest(requestEvent);

 pinMode(whiteLed, OUTPUT);
 pinMode(redLed, OUTPUT);
 pinMode(buttonPinUp, INPUT);
 pinMode(buttonPinDown, INPUT);

 pinMode(reedSwitchPin, INPUT);

}

void loop() {

if(digitalRead(buttonPinUp)){
    buttonUpPress = true; 
    
  }else if(digitalRead(buttonPinDown)){
    buttonDownPress = true;
  }

  if (buttonUpPress == true)
    digitalWrite(whiteLed, HIGH);

  if (buttonDownPress == true)
    digitalWrite(redLed, HIGH);  

  liftHere = digitalRead(reedSwitchPin);

  if(liftHere, HIGH){
    digitalWrite(ledPin, HIGH);
  }

}


void receiveEvent (){

 masterEtage = Wire.read();
 masterDirection = Wire.read();
 masterStop = Wire.read();
                  
}


void requestEvent(){
  
  Wire.write(buttonPinUp);
  Wire.write(buttonPinDown);
  Wire.write(liftHere);
     
}


void displayDigit(int digit){
  
  //Conditie voor segment a
  if (digit != 1 && digit != 4)
    digitalWrite(a, HIGH);

  //Conditie voor segment b
  if (digit != 5 && digit != 6)
    digitalWrite(b, HIGH);

  //Conditie voor segment c
  if (digit != 2)
    digitalWrite(c, HIGH);

  //Conditie voor segment d
  if (digit != 1 && digit != 4 && digit != 7)
    digitalWrite(d, HIGH);

  //Conditie voor segment e
  if (digit == 2 || digit == 6 || digit == 8 || digit == 0)
    digitalWrite(e, HIGH);

  //Conditie voor segment f
  if (digit != 1 && digit != 2 && digit != 3 && digit != 7)
    digitalWrite(f, HIGH);

  //Conditie voor segment g
  if (digit != 0 && digit != 1 && digit != 7)
    digitalWrite(g, HIGH);

}


void turnOff()
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}

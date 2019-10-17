#include <Wire.h>

#define led  9
#define reed_switch  13
#define button_led  6
#define button_led2  5
#define button_up  7
#define button_down  8
#define latchPin  12  //Pin connected to ST_CP of 74HC595
#define clockPin  10  //Pin connected to SH_CP of 74HC595 
#define dataPin  11   //Pin connected to DS of 74HC595 


int dat_array[7] = {
  B11111100,  // cijfer 0
  B00001100,  // cijfer 1
  B10110110,  // cijfer 2
  B10011110,  // cijfer 3
  B01001110,  // cijfer 4
  B11011010,  // cijfer 5
  B11111010}; // cijfer 6
  
bool travelling = false;
const int ROB_ETAGE = 5;

void setup ()
{
  Wire.begin(ROB_ETAGE);
  Wire.onReceive(receive_event);
  Wire.onRequest(request_event);
  Serial.println("hetwerkt");
  
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(reed_switch, INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(button_led, OUTPUT);
  pinMode(button_led2, OUTPUT);
  pinMode(button_up, INPUT);
  pinMode(button_down, INPUT);
  
}

void writeDigit(int i) 
 {
    digitalWrite (latchPin, LOW ); // latchPin low for duration of transmission
    shiftOut (dataPin, clockPin, MSBFIRST, i); // send data
    digitalWrite (latchPin, HIGH ); // latchPin high to save the data
 }

 void receive_event(){
    receive_etage = Wire.read();
    receive_direction = Wire.read();
    receive_stop = Wire.read();
 }

 void request_event(){
    
 }
void loop()
{
  if (digitalRead(reed_switch) == LOW) // als er iets voor de sensor staat
  { 
    writeDigit(dat_array[4]);
    digitalWrite(led, HIGH);
    digitalWrite(button_led, LOW);
    digitalWrite(button_led2, LOW);
  }
  else if (digitalRead(button_up) == HIGH) // als de 1e knop ingedrukt word
  { 
    travelling = true;
    digitalWrite(button_led, HIGH);
    writeDigit(dat_array[5]);
  }
  else if (digitalRead(button_down) == HIGH) // als de 2e knop ingedrukt word
  { 
    travelling = true;
    digitalWrite(button_led2, HIGH);
    writeDigit(dat_array[4]);
  }
  else
  {
    if(travelling == false)
    {
      digitalWrite(led, LOW);
      digitalWrite(button_led, LOW);
      digitalWrite(button_led2, LOW);
      writeDigit(dat_array[0]);
    }
  }
}

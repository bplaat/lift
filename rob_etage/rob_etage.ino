#include <wire.h>

int led = 9;
int reed_switch = 13;
int button_led = 6;
int button_led2 = 5;
int button_up = 7;
int button_down = 8;
const int latchPin = 12;  //Pin connected to ST_CP of 74HC595
const int clockPin = 10;  //Pin connected to SH_CP of 74HC595 
const int dataPin = 11;   //Pin connected to DS of 74HC595 
int dat_array[7] = {252, 12, 182, 158, 78, 218, 250};
bool travelling = false;

void setup ()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(reed_switch, INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(button_led, OUTPUT);
  pinMode(button_led2, OUTPUT);
  pinMode(button_up, INPUT);
  pinMode(button_down, INPUT_PULLUP);
  
}

void writeDigit(int i) 
 {
    digitalWrite (latchPin, LOW ); // latchPin low for duration of transmission
    shiftOut (dataPin, clockPin, MSBFIRST, i); // send data
    digitalWrite (latchPin, HIGH ); // latchPin high to save the data
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
  else if (digitalRead(button_down) == LOW) // als de 2e knop ingedrukt word
  { 
    travelling = true;
    digitalWrite(button_led2, HIGH);
    writeDigit(dat_array[4];
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

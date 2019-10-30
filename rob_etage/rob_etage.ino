#include <Wire.h>

#define led_pin  4
#define up_led  5
#define down_led  6
#define button_up  7
#define button_down  8
#define clock_pin  10  //Pin connected to SH_CP of 74HC595 
#define data_pin  11   //Pin connected to DS of 74HC595 
#define latch_pin  12  //Pin connected to ST_CP of 74HC595
#define reed_switch  13

#define ROB_ETAGE 5
#define ANSWERSIZE 3

bool button_up_press = false;
bool button_down_press = false;

int button_state_up = 0;
int button_state_down = 0;
int last_state_up = 0;
int last_state_down = 0;



int dat_array[] = 
{
    B11111100,  // cijfer 0
    B00001100,  // cijfer 1
    B10110110,  // cijfer 2
    B10011110,  // cijfer 3
    B01001110,  // cijfer 4
    B11011010,  // cijfer 5
    B11111010   // cijfer 6
}; 

int lift_etage = 0;
int lift_state = 0;
int lift_here = 0;
int lift_stop = 0;
int lift_stop_accepted = 0;

#define LIFTNOTMOVING 0
#define LIFTMOVING 1
#define LIFTWAITING 2

#define NOSTOP 0
#define STOPFORUP -1
#define STOPFORDOWN 1

bool blink_state = false;

void writeDigit(int i) 
{
    digitalWrite (latch_pin, LOW ); // latchPin low for duration of transmission
    shiftOut (data_pin, clock_pin, MSBFIRST, dat_array[i]); // send data
    digitalWrite (latch_pin, HIGH ); // latchPin high to save the data
}

void receive_event(){
    Serial.println("receive");
    lift_etage = Wire.read();
    lift_state = Wire.read();
        if (lift_state == LIFTWAITING && lift_here) {
            lift_stop_accepted = 0;
        }
        int new_lift_stop_accepted = Wire.read();
        if (new_lift_stop_accepted != 0) {
            lift_stop_accepted = new_lift_stop_accepted;
}

void request_event(){
    Serial.println("request");
    Wire.write(1);
    Wire.write(lift_here);
    Wire.write(lift_stop);
    if (lift_stop != 0) {
        lift_stop = 0;
    }
}

void setup ()
{
    
    Serial.begin(9600);
    Serial.println("Rob etage klaar");
    Wire.begin(ROB_ETAGE);
    Wire.onReceive(receive_event);
    Wire.onRequest(request_event);
    pinMode(led_pin, OUTPUT);
    pinMode(reed_switch, INPUT_PULLUP);
    pinMode(latch_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    pinMode(up_led, OUTPUT);
    pinMode(down_led, OUTPUT);
    pinMode(button_up, INPUT);
    pinMode(button_down, INPUT);

}

void loop()
{
    lift_here = digitalRead(reed_switch);

    if (lift_here == HIGH) {
    Serial.println("Reed Rob active");
    Serial.println("Led for arrival on");
    lift_here = 1;
    // writeDigit(2);
    digitalWrite(led_pin, HIGH);
    digitalWrite(up_led, LOW);
    digitalWrite(down_led, LOW);
  } else {
    digitalWrite(led_pin, LOW);
    writeDigit(lift_etage);
  }

  // blink display
  if (lift_state == LIFTMOVING) {
    if (blink_state == 0 && millis() - blink_time > 100) {
      blink_state = 1;
      blink_time = millis();
    }
    if (blink_state == 1 && millis() - blink_time > 100) {
      blink_state = 0;
      blink_time = millis();
    }

    // button for stop up
    button_state_up = digitalRead(button_up);
    if (button_state_up != last_state_up) {
      if (button_state_up == LOW) {
        button_up_press = true;
        lift_stop = STOPFORUP;
        Serial.println("Up pressed");
        digitalWrite(up_led, HIGH);
        Serial.println("Up led on");
      } else {
        Serial.println("Up not pressed");
      }
    }
    last_state_up = button_state_up;

    // button for stop down
    button_state_down = digitalRead(button_down);
    if (button_state_down != last_state_down) {
      if (button_state_down == LOW) {
        button_down_press = true;
        lift_stop = STOPFORDOWN;
        Serial.println("Down pressed");
        digitalWrite(redLed, HIGH);
        Serial.println("Down led on");
      } else {
        Serial.println("Down not pressed");
      }
    }
    last_state_down = button_state_down;
  }

}

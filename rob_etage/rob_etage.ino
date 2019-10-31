#include <Wire.h>

#define led_pin  4
#define led_up  5
#define led_down  6
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



//const byte dat_array[] = 
//{
//    B11111100,  // cijfer 0
//    B00001100,  // cijfer 1
//    B10110110,  // cijfer 2
//    B10011110,  // cijfer 3
//    B01001110,  // cijfer 4
//    B11011010,  // cijfer 5
//    B11111010   // cijfer 6
//}; 
//
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

int lift_etage = 0;
int lift_state = 0;
int lift_here = 0;
int lift_stop = 0;
int lift_stop_accepted = 0;

#define LIFT_NOT_MOVING 0
#define LIFT_MOVING 1
#define LIFT_WAITING 2

#define NO_STOP 0
#define STOP_DOWN 1
#define STOP_UP 2

bool blink_state = false;
int blink_time;

void write_digit(int i) 
{
    digitalWrite (latch_pin, LOW ); // latchPin low for duration of transmission
    shiftOut (data_pin, clock_pin, MSBFIRST, dat_array[i]); // send data
    digitalWrite (latch_pin, HIGH ); // latchPin high to save the data
}

void receive_event(){
    Serial.println("receive");
    lift_etage = Wire.read();
    lift_state = Wire.read();
        if (lift_state == LIFT_WAITING && lift_here) {
            lift_stop_accepted = 0;
        }
        int new_lift_stop_accepted = Wire.read();
        if (new_lift_stop_accepted != 0) {
            lift_stop_accepted = new_lift_stop_accepted;
  }
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
    pinMode(led_up, OUTPUT);
    pinMode(led_down, OUTPUT);
    pinMode(button_up, INPUT);
    pinMode(button_down, INPUT);

    digitalWrite(led_up, LOW);
    digitalWrite(led_down, LOW);
    digitalWrite(led_pin, LOW);
}

void loop()
{
    lift_here = digitalRead(reed_switch);

       if (lift_here != LIFTMOVING && lift_here {
       digitalWrite(led_pin, HIGH);
       write_digit(lift_etage);
    } 
    else {
      digitalWrite(led_pin, LOW);
      Serial.println("Lift not here");
      write_digit (lift_etage);
    }

  // blink display
  if (lift_state == LIFT_MOVING) {
    if (blink_state == 0 && millis() - blink_time > 100) {
      blink_state = 1;
      blink_time = millis();
    }
    if (blink_state == 1 && millis() - blink_time > 100) {
      blink_state = 0;
      blink_time = millis();
    }
  }
   
    // button for stop up
    if(lift_stop_accepted == STOP_UP){
      digitalWrite(led_up, HIGH);
    }
    button_state_up = digitalRead(button_up, HIGH);
    if(button_state_up == LOW &&
     !(lift_state == LIFT_WAITING && lift_here) 
     && lift_stop_accepted == 0 && lift_stop == 0){
       lift_stop = STOP_UP;
    }

    // button for stop down
    if (lift_stop_accepted == STOP_DOWN) {
      digitalWrite(led_down, HIGH);
    }

    button_state_down = digitalRead(button_down, HIGH);
    if(button_state_down == LOW && 
    !(lift_state == LIFT_WAITING && lift_here) 
    && lift_stop_accepted == 0 && lift_stop == 0){
      lift_stop = STOP_DOWN;
    }
     
  }

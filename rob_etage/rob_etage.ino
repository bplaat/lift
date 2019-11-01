#include <Wire.h>

#define LED_PIN  4
#define LED_UP  5
#define LED_DOWN  6
#define BUTTON_UP  7
#define BUTTON_DOWN  8
#define CLOCK_PIN  10  //Pin connected to SH_CP of 74HC595 
#define DATA_PIN  11   //Pin connected to DS of 74HC595 
#define LATCH_PIN  12  //Pin connected to ST_CP of 74HC595
#define REED_SWITCH  13

#define ROB_ETAGE 4
#define ANSWER_SIZE 3

bool button_up_press = false;
bool button_down_press = false;

int button_state_up = 0;
int button_state_down = 0;
int last_state_up = 0;
int last_state_down = 0;

const byte dat_array[] = {
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
int blink_time = millis();

void write_digit(int i) 
{
  digitalWrite (LATCH_PIN, LOW ); // latchPin low for duration of transmission
  shiftOut (DATA_PIN, CLOCK_PIN, MSBFIRST, dat_array[i]); // send data
  digitalWrite (LATCH_PIN, HIGH ); // latchPin high to save the data
}

void receive_event()
{
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

void request_event()
{
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
  pinMode(LED_PIN, OUTPUT);
  pinMode(REED_SWITCH, INPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_UP, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);

  digitalWrite(LED_UP, LOW);
  digitalWrite(LED_DOWN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void clear_digit() {
  digitalWrite(LATCH_PIN, LOW);
}

void loop()
{
  lift_here = !digitalRead(REED_SWITCH);

  if (lift_state != LIFT_MOVING && lift_here) {
    digitalWrite(LED_PIN, HIGH);
  } 
  else {
    digitalWrite(LED_PIN, LOW);
  }

  if (lift_state == LIFT_MOVING) 
  {
    if (blink_state == 0 && millis() - blink_time > 200) {
      blink_state = 1;
      blink_time = millis();
    }
    if (blink_state == 1 && millis() - blink_time > 200) {
      blink_state = 0;
      blink_time = millis();
    }
  
    if (blink_state == 1) 
    {
      write_digit(lift_etage);
    } 
    else 
    {
      clear_digit();
    }
  }
   
  else 
  {
    write_digit(lift_etage);
  }

  if(lift_stop_accepted == STOP_UP)
  {
    digitalWrite(LED_UP, HIGH);
  }

  button_state_up = digitalRead(BUTTON_UP);

  if(button_state_up == LOW &&
  !(lift_state == LIFT_WAITING && lift_here) 
  && lift_stop_accepted == 0 && lift_stop == 0)
  {
    lift_stop = STOP_UP;
  }

  if (lift_stop_accepted == STOP_DOWN) 
  {
    digitalWrite(LED_DOWN, HIGH);
  }

  button_state_down = digitalRead(BUTTON_DOWN);

  if(button_state_down == LOW && 
  !(lift_state == LIFT_WAITING && lift_here) 
  && lift_stop_accepted == 0 && lift_stop == 0)
  {
    lift_stop = STOP_DOWN;
  }

}

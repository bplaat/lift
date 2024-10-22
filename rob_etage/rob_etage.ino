#include <Wire.h>

// Defining all the in and outputs
#define LED_PIN  4
#define LED_UP  5
#define LED_DOWN  6
#define BUTTON_UP  7
#define BUTTON_DOWN  8
#define CLOCK_PIN  10  //Pin connected to SH_CP of 74HC595 
#define DATA_PIN  11   //Pin connected to DS of 74HC595 
#define LATCH_PIN  12  //Pin connected to ST_CP of 74HC595
#define REED_SWITCH  13

// Defining the floor number and and the byte answer size to the master
#define ROB_FLOOR 4
#define ANSWER_SIZE 3
#define FLOOR_OFFSET 10 

int last_state_up = 0;
int last_state_down = 0;

// Bit values for the correct displayment of numbers on 7 segment
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

// Multiple variables for various applications
int lift_floor = 0;
int lift_state = 0;
int lift_here = 0;
int lift_stop = 0;
bool send_for_lift = 0;
int lift_stop_accepted = 0;

// Defining the different states of the lift and stops for I2C
#define LIFT_NOT_MOVING 0
#define LIFT_MOVING 1
#define LIFT_WAITING 2

#define NO_STOP 0
#define STOP_DOWN 1
#define STOP_UP 2

// Variables for the 7 segment display blink state
bool blink_state = false;
unsigned long blink_time = millis();

// Function to write a number to the 7 segment
void write_digit(int i) 
{
  digitalWrite (LATCH_PIN, LOW ); // latchPin low for duration of transmission
  shiftOut (DATA_PIN, CLOCK_PIN, MSBFIRST, dat_array[i]); // send data
  digitalWrite (LATCH_PIN, HIGH ); // latchPin high to save the data
}

// I2C function to receive data from master
void receive_event()
{
  lift_floor = Wire.read();
  lift_state = Wire.read();
  int new_lift_stop_accepted = Wire.read();
  if (new_lift_stop_accepted != 0) {
    lift_stop_accepted = new_lift_stop_accepted;
    send_for_lift = 0;
  }
}

// I2C function to respond data to master
void request_event()
{
  Wire.write(1);
  Wire.write(lift_here);
  Wire.write(lift_stop);
  if (lift_stop != 0) {
    lift_stop = 0;
    send_for_lift = 1;
  }
}

void setup ()
{
  // Begin I2C protocol
  Wire.begin(ROB_FLOOR + FLOOR_OFFSET);
  Wire.onReceive(receive_event);
  Wire.onRequest(request_event);

  // Declare inputs and outputs as such
  pinMode(LED_PIN, OUTPUT);
  pinMode(REED_SWITCH, INPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_UP, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
}

// Function to clear the last number from the 7 segment display
void clear_digit() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut (DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  digitalWrite (LATCH_PIN, HIGH );
}

void loop()
{
  // Determine whether the lift is here by way of reed switch
  lift_here = !digitalRead(REED_SWITCH);

// Turn LED on or off depending on whether the lift is here
  if (lift_state != LIFT_MOVING && lift_here) {
    digitalWrite(LED_PIN, HIGH);
  } 
  else {
    digitalWrite(LED_PIN, LOW);
  }

// Determine not to assign a new stop if the lift is already here
  if (lift_state == LIFT_WAITING && lift_here) {
    lift_stop_accepted = 0;
  }

// If statements to make the 7 segment display blink when the lift is moving
  if (lift_state == LIFT_MOVING) 
  {
    if (blink_state == 0 && millis() - blink_time > 100) {
      blink_state = 1;
      blink_time = millis();
    }
    if (blink_state == 1 && millis() - blink_time > 100) {
      blink_state = 0;
      blink_time = millis();
    }
  
    if (blink_state == 1) 
    {
      write_digit(lift_floor);
    } 
    else 
    {
      clear_digit();
    }
  }
  else 
  {
    write_digit(lift_floor);
  }

// Code for the up button to light up if the button is pressed and the stop is accepted
  if(lift_stop_accepted == STOP_UP)
  {
    digitalWrite(LED_UP, HIGH);
  } 
  else {
    digitalWrite(LED_UP, LOW);    
  }

  if(digitalRead(BUTTON_UP) == LOW &&
  !(lift_state == LIFT_WAITING && lift_here) 
  && lift_stop_accepted == 0 && send_for_lift == 0 && lift_stop == 0)
  {
    lift_stop = STOP_UP;
  }

// Code for the down button to light up if the button is pressed and the stop is accepted.
  if (lift_stop_accepted == STOP_DOWN) 
  {
    digitalWrite(LED_DOWN, HIGH);
  } 
  else {
    digitalWrite(LED_DOWN, LOW);
  }

  if(digitalRead(BUTTON_DOWN) == LOW && 
  !(lift_state == LIFT_WAITING && lift_here) 
  && lift_stop_accepted == 0 && send_for_lift == 0 && lift_stop == 0)
  {
    lift_stop = STOP_DOWN;
  }

}

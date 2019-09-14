// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

// ### Simple Digit Display Library for the Arduino ###
// Made by Bastiaan van der Plaat

// The pins you connect the segment display to from a to g
byte digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

// The segments for each digit that will be turn on
byte digit_display_digits[10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

// Function the initialize the pins
void digit_display_init(void) {
  for (byte i = 0; i < 7; i++) {
    pinMode(digit_display_pins[i], OUTPUT);
  }
}

// Function that sets a single digit from 0 to 9
void digit_display_set_digit(byte digit) {
  if (digit > 9) return;
  for (byte i = 0; i < 7; i++) {
    digitalWrite(digit_display_pins[i], (digit_display_digits[digit] >> i) & 1 ? HIGH : LOW);
  }
}

// ### Main code ###

#define BUTTON_PIN 9

byte counter = 0;
bool button_pressed = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Bastiaan's Etage");

  // Init digit display
  digit_display_init();

  // Set pin mode for button to input pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Set digit display to counter
  digit_display_set_digit(counter);

  // If button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!button_pressed) {
      button_pressed = true;

      // Increment the counter
      if (counter == 9) {
        counter = 0;
      } else {
        counter++;
      }
    }
  } else{
    button_pressed = false;
  }
}

const int a = 8;  //segment "a"
const int b = 9;  //segment "b"
const int c = 4;  //segment "c"
const int d = 5;  //segment "d"
const int e = 6;  //segment "e"
const int f = 2;  //segment "f"
const int g = 3;  //segment "g"

bool bPress = false;
const int buttonPin = 10;

// Variables die zullen veranderen:
int buttonPushCounter = 0;   // teller voor de knop
int buttonState = 0;         // huidige staat van de knop
int lastButtonState = 0;     // vorige staat van de knop

void setup() {

  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  pinMode( buttonPin , INPUT_PULLUP );
  Serial.begin(9600);
  displayDigit(buttonPushCounter);
}

void loop() {

  buttonState = digitalRead(buttonPin);

  // vergelijkt de staat van de knop met de vorige
  if (buttonState != lastButtonState) {
    // als de state veranderd is, pas de teller aan
    if (buttonState == LOW) {
      // als de huidige state HIGH is dan is de knop van off naar on gegaan:
      bPress = true;
      buttonPushCounter++;
      if ( buttonPushCounter > 9)
        buttonPushCounter = 0 ;
      Serial.println("on");

    } else {
      // als de huidige state LOW is dan is de knop van on naar off gegaan:
      Serial.println("off");
    }
    delay(100);
  }
  // bewaar de huidige state als de vorige state voor de loop
  lastButtonState = buttonState;

  if ( bPress ) {
    turnOff();
    displayDigit(buttonPushCounter);
  }

}


void displayDigit(int digit)
{
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

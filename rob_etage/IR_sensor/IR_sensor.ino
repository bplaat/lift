const int ProxSensor=A0;
int inputVal = 0;

void setup() 
{                
  pinMode(3, OUTPUT);          // Pin 13 has an LED connected on most Arduino boards:  
  pinMode(ProxSensor,INPUT);    //Pin 2 is connected to the output of proximity sensor
  Serial.begin(9600);
}

void loop() 
{
  if(digitalRead(ProxSensor)==HIGH)      //Check the sensor output
  {
    digitalWrite(3, HIGH);   // set the LED on
  }
  else
  {
    digitalWrite(3, LOW);    // set the LED off
  }
inputVal = digitalRead(ProxSensor);
Serial.println(inputVal);
delay(1000);              // wait for a second
}

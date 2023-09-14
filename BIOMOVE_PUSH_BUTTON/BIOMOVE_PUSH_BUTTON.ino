#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

void setup()
{
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  //pinMode(WB_IO1, INPUT); // SIMPLE INPUT ONLY WITH PHYSICAL RESISTOR CONNECTED AS PULL-UP

  pinMode(WB_IO1, INPUT_PULLUP);

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  digitalWrite(WB_IO1, HIGH); // Optional Internal Pull-Up
  Serial.begin(115200);
}

void loop () {
  //read the pushbutton value into a variable

  int myButton = digitalRead(WB_IO1);
  //print out the value of the pushbutton

  Serial.println(myButton);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  
  if (myButton == LOW) {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, LOW);

  } else {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}

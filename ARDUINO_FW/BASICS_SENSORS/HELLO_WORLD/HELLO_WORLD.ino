#include <Arduino.h>
#include <Wire.h>

void setup() {

  Serial.begin(115200); // initialize Serial port at 115200 baudrate
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);

}

void loop() {

  digitalWrite(LED_GREEN, LOW);
  delay(1000);
  digitalWrite(LED_GREEN, HIGH);
  delay(1000);

  Serial.print("hello ");    // print string hello
  Serial.println("world");   // print string world on a new line

}

#include "Arduino.h"
#include "Wire.h"
#define SDA_PIN WB_I2C1_SDA
#define SCL_PIN WB_I2C1_SCL

// Use bit banging to get higher speed output
#define HARDWARE_I2C 1

int rc;

static uint8_t ucBuffer[1024];

void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(100);
  Serial.begin(115200);
  time_t timeout = millis();
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  delay(500);
  Serial.println("\n\nI2C Scanner...");
  Wire.begin();
  //Wire.setClock(400000);

  byte error = Wire.endTransmission();

}

char result[128];


void loop() {
  byte error, addr;
  uint8_t nDevices, ix = 0;
  Serial.println("\nI2C scan in progress...");
  nDevices = 0;
  Serial.print("   |   .0   .1   .2   .3   .4   .5   .6   .7   .8   .9   .A   .B   .C   .D   .E   .F\n");
  Serial.print("-------------------------------------------------------------------------------------\n0. |   .  ");
  char memo[64];
  char buff[32];

 
  for (addr = 1; addr < 128; addr++) {
    Wire.beginTransmission(addr);
    error = Wire.endTransmission();
    // Wire.beginTransmission(addr);
    // error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("0x");
      if (addr < 16) Serial.write('0');
      Serial.print(addr, HEX);
      result[ix++] = addr;

      nDevices++;


    } else {
      Serial.print("  . ");
    } Serial.write(' ');
    if (addr > 0 && (addr + 1) % 16 == 0 && addr < 127) {
      Serial.write('\n');
      Serial.print(addr / 16 + 1);
      Serial.print(". | ");
    }
  }
  Serial.println("\n-------------------------------------------------------------------------------------");
  Serial.println("I2C devices found: " + String(nDevices));

  delay(5000);
}

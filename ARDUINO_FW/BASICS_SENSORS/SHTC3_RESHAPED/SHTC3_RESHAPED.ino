/**
   @file RAK1901_Temperature_Humidity_SHTC3.ino
   @author rakwireless.com
   @brief Setup and read values from a SHTC3 temperature and humidity sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include "sensor_Read.h"

void setup() {
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }

  shtc3_begin();

}

void loop() {
  shtc3_read_data();
  delay(1000);
}

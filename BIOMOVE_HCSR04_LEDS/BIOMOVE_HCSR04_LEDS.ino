/**
   @file RAK12007_OLED_Show_Distance.ino
   @author rakwireless.com
   @brief use ultrasonic measure distance and display the distance value on OLED example
   velocity of sound =331.6+0.6T(m/s),(T is the Celsius temperature,331.6 m/sIs the speed of sound waves
   traveling through air at a temperature of 0 degrees Celsius).
   @version 0.1
   @date 2021-06-17
   @copyright Copyright (c) 2021
*/

#include <Wire.h>

#ifdef RAK4630
#define BOARD "RAK4631 "
#define  RAK4631_BOARD true
#else
#define  RAK4631_BOARD false
#endif




#define TRIG WB_I2C1_SDA
#define ECHO WB_IO1
#define PD   WB_IO5   //power done control （=1 power done，=0 power on） 

#define TIME_OUT  24125 //max measure distance is 4m,the velocity of sound is 331.6m/s in 0℃,TIME_OUT=4*2/331.6*1000000=24215us

float ratio = 346.6 / 1000 / 2; //velocity of sound =331.6+0.6*25℃(m/s),(Indoor temperature about 25℃)

long int duration_time();  //measure high level time

void setup()
{
  Serial.begin(115200);
  time_t timeout = millis();
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  pinMode(ECHO, INPUT);   // Echo Pin of Ultrasonic Sensor is an Input
  pinMode(TRIG, OUTPUT);  // Trigger Pin of Ultrasonic Sensor is an Output
  // pinMode(PD, OUTPUT);    // power done control pin is an Output
  digitalWrite(TRIG, LOW);
  // digitalWrite(PD,LOW);
  pinMode(LED_BLUE, OUTPUT);   // The LED is an Output
  pinMode(LED_GREEN, OUTPUT);   // The LED is an Output

  Serial.println("========================");
  Serial.println("    RAK12007 test");
  Serial.println("========================");
}
void loop()
{
  long int duration, mm;
  digitalWrite(LED_BLUE, HIGH);
  duration = duration_time();
  if (duration > 0)
  {
    mm = duration * ratio; //Test distance = (high level time×velocity of sound (340M/S) / 2,
    digitalWrite(LED_BLUE, LOW);
    Serial.print(mm);
    Serial.print("mm");
    Serial.println();
    if (mm < 1000) {  // This is where the LED On/Off happens
      digitalWrite(LED_GREEN, HIGH); // When the Red condition is met, the Green LED should turn off
    }
    else {
      digitalWrite(LED_GREEN, LOW);
    }
  }
  else
  {
    Serial.println("Out of range");
  }
  delay(100);
}
long int duration_time()
{
  long int respondTime;
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(12);   //pull high time need over 10us
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
  respondTime = pulseIn(ECHO, HIGH); // microseconds
  delay(33);
  if (RAK4631_BOARD)
  {
    respondTime = respondTime * 0.7726; // Time calibration factor is 0.7726,to get real time microseconds for 4631board
  }
  Serial.printf("respond time is %d\r\n", respondTime);

  if ((respondTime > 0) && (respondTime < TIME_OUT)) //ECHO pin max timeout is 33000us according it's datasheet
  {
    return respondTime;
  }
  else
  {
    return -1;
  }
}

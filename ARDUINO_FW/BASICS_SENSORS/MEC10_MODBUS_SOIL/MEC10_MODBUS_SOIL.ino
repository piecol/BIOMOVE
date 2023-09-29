#include <Arduino.h>
//#include <LoRaWan-RAK4630.h> // Click here to get the library: http://librarymanager/All#SX126x

#include <ArduinoRS485.h>   //Click here to get the library: http://librarymanager/All#ArduinoRS485
#include <ArduinoModbus.h>  //Click here to get the library: http://librarymanager/All#ArduinoModbus
#include <SPI.h>

//#define HOLDING_REGISTERS 3

float temperature;
float humidity;
int EC;

void setup() {
  // put your setup code here, to run once:

  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }


  //ModbusRTUClient.begin(9600);
  if (!ModbusRTUClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1)
      ;
  }
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t i = 0;
  short raw_conductivity;
  short raw_temperature;
  short raw_humidity;
  short all;

  /* RS485 Power On */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(2000);
  /* RS485 Power On */
  Serial.println(get_soil_temperature());


  raw_temperature = get_soil_temperature();
  raw_humidity = get_soil_humidity();
  raw_conductivity = get_soil_conductivity();

  //all = get_all();

  /* RS485 Power Off */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, LOW);
  delay(100);
  /* RS485 Power Off */

  
  Serial.printf("\n===== RAW VALUES\n");
  Serial.printf("-----raw_temperature   = %d\n", raw_temperature);
  Serial.printf("-----raw_humidity      = %d\n", raw_humidity);
  Serial.printf("-----raw_conductivity  = %d\n", raw_conductivity);
  Serial.printf("\n===== TRANSFORMED VALUES\n");
  Serial.printf("-----temperature       = %0.1f C°\n", raw_temperature / 100.0);
  Serial.printf("-----humidity          = %0.1f %%\n", raw_humidity / 100.0);
  Serial.printf("-----conductivity      = %d uS\n", raw_conductivity);

  delay(2000);
  
  /* @pico TROUBLESHOOTING

                // send a Holding registers read request to (slave) id 1, for 3 registers
                if (!ModbusRTUClient.requestFrom(0x01, HOLDING_REGISTERS, 0x00, 0x03)) {
                  Serial.print("failed to read registers! ");
                  Serial.println(ModbusRTUClient.lastError());
                } else {
                  // If the request succeeds, the sensor sends the readings, that are
                  // stored in the holding registers. The read() method can be used to
                  // get the raw temperature, humidity and conductivity values.
                  short raw_temperature = ModbusRTUClient.read();
                  short raw_humidity = ModbusRTUClient.read();
                  short raw_conductivity = ModbusRTUClient.read();


                  // To get the temperature in Celsius and the humidity reading as
                  // a percentage, divide the raw value by 100.0.
                  temperature = raw_temperature / 100.0;
                  humidity = raw_humidity / 100.0;
                  EC = raw_conductivity ;

                  Serial.println("");
                  Serial.println(temperature);
                  Serial.println(humidity);
                  Serial.println(EC);
                  Serial.println("=========");
                  Serial.println(raw_temperature, HEX);
                  Serial.println(raw_humidity, HEX);
                  Serial.println(raw_conductivity, HEX);
                } */

}



static short read_reg(int reg_address) {
  short reg_value;

  if (!ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, reg_address, 1)) {
    Serial.print("failed to read registers! ");
    Serial.println(ModbusRTUClient.lastError());
  } else {
    // If the request succeeds, the sensor sends the readings, that are
    // stored in the holding registers. The read() method can be used to
    // get the raw humidity temperature values.
    reg_value = ModbusRTUClient.read();
    Serial.println(reg_value);
  }
  return reg_value;
}


static short get_soil_temperature(void) {
  return read_reg(0x0000);
}

static short get_soil_humidity(void) {
  return read_reg(0x0001);
}

static short get_soil_conductivity(void) {
  return read_reg(0x0002);
}

static short get_all(void) {
  return read_reg(0x0000);
}

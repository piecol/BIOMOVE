/**
   @file ble_environment_node.ino
   @author rakwireless.com
   @brief This sketch demonstrate how to get environment data from BME680
      and send the data to ble gateway with ble gatt.
      ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
   @version 0.1
   @date 2020-07-28
   @copyright Copyright (c) 2020
**/

#include <bluefruit.h>
#include <Wire.h>
#include "SparkFun_SHTC3.h"  //Click here to get the library: http://librarymanager/All#SparkFun_SHTC3

SHTC3 mySHTC3;  // Declare an instance of the SHTC3 class


float temperature = 0;
float humidity = 0;

/* Environment Service Definitions
   Environment Monitor Service:  0x181A
   Pressure Measurement Char: 0x2A6D
   Temperature Measurement Char: 0x2A6E
   humidity Measurement Char: 0x2A6F
*/
BLEService envms = BLEService(UUID16_SVC_ENVIRONMENTAL_SENSING);
//BLECharacteristic pressuremc = BLECharacteristic(0x2A6D);
BLECharacteristic temprtmc = BLECharacteristic(0x2A6E);
BLECharacteristic humidmc = BLECharacteristic(0x2A6F); // humidity

BLEDis bledis; // DIS (Device Information Service) helper class instance
BLEBas blebas; // BAS (Battery Service) helper class instance
void Sensor_SHTC3_Init(void);
void Sensor_SHTC3_GetData(void);


void Sensor_SHTC3_Init(void) {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  
  Wire.begin();
  mySHTC3.begin();
  Wire.setClock(400000);             // The sensor is listed to work up to 1 MHz I2C speed, but the I2C clock speed is global for all sensors on that bus so using 400kHz or 100kHz is recommended
  Serial.println();
  if (mySHTC3.passIDcrc) // Whenever data is received the associated checksum is calculated and verified so you can be sure the data is true
  {            // The checksum pass indicators are: passIDcrc, passRHcrc, and passTcrc for the ID, RH, and T readings respectively
    Serial.print("ID Passed Checksum. ");
    Serial.print("Device ID: 0b");
    Serial.println(mySHTC3.ID, BIN); // The 16-bit device ID can be accessed as a member variable of the object
  }
  else
  {
    Serial.println("ID Checksum Failed. ");
  }
}



void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include Environment Service UUID
  Bluefruit.Advertising.addService(envms);

  // Include Name
  Bluefruit.Advertising.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)
     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);	 // number of seconds in fast mode
  Bluefruit.Advertising.start(0);				 // 0 = Don't stop advertising after n seconds
}

void setupEnvm(void)
{
  // Configure the Environment service
  // See: https://www.bluetooth.com/specifications/assigned-numbers/environmental-sensing-service-characteristics/
  // Supported Characteristics:
  // Name                         UUID    Requirement Properties
  // ---------------------------- ------  ----------- ----------
  // Pressure Measurement: 0x2A6D
  // Temperature Measurement: 0x2A6E
  // humidity Measurement: 0x2A6F
  envms.begin();

  // Note: You must call .begin() on the BLEService before calling .begin() on
  // any characteristic(s) within that service definition.. Calling .begin() on
  // a BLECharacteristic will cause it to be added to the last BLEService that
  // was 'begin()'ed!

  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml
  // Properties = Notify


  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature.xml
  // Properties = Notify
  temprtmc.setProperties(CHR_PROPS_NOTIFY);
  temprtmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  temprtmc.setFixedLen(2);
  temprtmc.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
  temprtmc.begin();
  uint8_t tmpdata[2] = {0x00}; // Set the characteristic to use 8-bit values, with the sensor connected and detected
  temprtmc.write(tmpdata, 2);

  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.humidity.xml
  // Properties = Notify
  humidmc.setProperties(CHR_PROPS_READ);
  humidmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  humidmc.setFixedLen(2);
  humidmc.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
  humidmc.begin();
  uint8_t humiddata[2] = {0x00}; // Set the characteristic to use 8-bit values, with the sensor connected and detected
  humidmc.write(humiddata, 2);
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection *connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = {0};
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
   Callback invoked when a connection is dropped
   @param conn_handle connection where this event happens
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void)conn_handle;
  (void)reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic *chr, uint16_t cccd_value)
{
  // Display the raw request packet
  Serial.print("CCCD Updated: ");
  //Serial.printBuffer(request->data, request->len);
  Serial.print(cccd_value);
  Serial.println("");

  // Check the characteristic this CCCD update is associated with in case
  // this handler is used for multiple CCCD records.

  if (chr->uuid == temprtmc.uuid)
  {
    if (chr->notifyEnabled(conn_hdl))
    {
      Serial.println("Temperature Measurement 'Notify' enabled");
    }
    else
    {
      Serial.println("Temperature Measurement 'Notify' disabled");
    }
  }
  else if (chr->uuid == humidmc.uuid)
  {
    if (chr->notifyEnabled(conn_hdl))
    {
      Serial.println("Humidity Measurement 'Notify' enabled");
    }
    else
    {
      Serial.println("Humidity Measurement 'Notify' disabled");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  // while ( !Serial ) delay(10);   // for nrf52840 with native usb

  /* bme680 init */
  Sensor_SHTC3_Init();

  Serial.println("RAK Wisnode EnvM Example");
  Serial.println("-----------------------\n");

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit module");
  Bluefruit.begin();

  // Set the advertised device name (keep it short!)
  Serial.println("Setting Device Name to 'RAKwisnode EnvM'");
  Bluefruit.setName("RAKwisnode EnvM");

  Bluefruit.setTxPower(4);

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure and Start the Device Information Service
  Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Start the BLE Battery Service and set it to 100%
  Serial.println("Configuring the Battery Service");
  blebas.begin();
  blebas.write(100);

  // Setup the Environment service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Environment Monitor Service");
  setupEnvm();

  // Setup the advertising packet(s)
  Serial.println("Setting up the advertising payload(s)");
  startAdv();

  Serial.println("\nAdvertising");
}

void loop()
{
  digitalToggle(LED_GREEN);
    mySHTC3.update();


    temperature = mySHTC3.toDegC();
    humidity = mySHTC3.toPercent();
  

  if (Bluefruit.connected())
  {
    uint8_t tmpdata[2] = {0x0}; // little-endian
    tmpdata[1] = (uint8_t)(((int)(temperature * 100) & 0xff00) >> 8);
    tmpdata[0] = (uint8_t)((int)(temperature * 100) & 0xff);

    uint8_t humiddata[2] = {0x0}; // little-endian
    humiddata[1] = (uint8_t)(((int)(humidity * 100) & 0xff00) >> 8);
    humiddata[0] = (uint8_t)((int)(humidity * 100) & 0xff);


    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent

    if (temprtmc.write(tmpdata, sizeof(tmpdata)))
    {
      Serial.print("Temperature Measurement updated to: ");
      Serial.println(temperature);
    }
    if (humidmc.write(humiddata, sizeof(humiddata)))
    {
      Serial.print("Humidityrature Measurement updated to: ");
      Serial.println(humidity);
    }
  }

  delay(2000);
}

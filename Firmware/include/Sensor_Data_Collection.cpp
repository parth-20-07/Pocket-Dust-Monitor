#ifndef Sensor_Data_Collection_cpp
#define Sensor_Data_Collection_cpp

#include "Arduino.h"
#include "Pin_Connection.h"
#include "Variable_Declaration.h"

// #define HTU21D_TEMP_HUMIDITY_SENSOR
// #define MHZ19C_CO2_SENSOR
// #define TSL2561_LUX_SENSOR
#define PMS_DUST_SENSOR

/* -------------------------------------------------------------------------- */
/*                              BASIC DEFINITION                              */
/* -------------------------------------------------------------------------- */
#ifdef HTU21D_TEMP_HUMIDITY_SENSOR
#include <Wire.h>
#include "HTU21D.h"
HTU21D temperature_and_humidity_sensor; // Create an instance of the object
#endif

#ifdef MHZ19C_CO2_SENSOR
#include "MHZ19.h"
#include "HardwareSerial.h"
HardwareSerial MHZ19CSerial(2);
MHZ19 carbondioxide_sensor; // Constructor for library
#endif

#ifdef TSL2561_LUX_SENSOR
#include <Wire.h>
#include <SparkFunTSL2561.h>
SFE_TSL2561 light_sensor; // Create an SFE_TSL2561 object
// Global variables:
boolean gain;    // Gain setting, 0 = X1, 1 = X16;
unsigned int ms; // Integration ("shutter") time in milliseconds
#endif

#ifdef PMS_DUST_SENSOR
#include <PMS.h>
#include "SoftwareSerial.h"
SoftwareSerial PMSerial2(PMS_RX, PMS_TX);
PMS pms(PMSerial2);
PMS::DATA data;
#define VALUE_LENGTH 5
#define MINIMUM_WIDTH 1
#define NUM_AFTER_DECIMAL 0
char pm_sp_1_0[VALUE_LENGTH], pm_sp_2_5[VALUE_LENGTH], pm_sp_10_0[VALUE_LENGTH]; // Variable to store standard atmospheric particles values
char pm_ae_1_0[VALUE_LENGTH], pm_ae_2_5[VALUE_LENGTH], pm_ae_10_0[VALUE_LENGTH]; // Variable to store Atmospheric environment values
#endif

/* -------------------------------------------------------------------------- */
/*                             FUNCTION DEFINITION                            */
/* -------------------------------------------------------------------------- */
void setup_sensors(void)
{
    Serial.println("Setting up Sensors");
#ifdef HTU21D_TEMP_HUMIDITY_SENSOR
    Serial.println("Setting up Temperature and Humidity Sensor");
    temperature_and_humidity_sensor.begin();
#endif

#ifdef MHZ19C_CO2_SENSOR
    Serial.println("Setting up CO2 Sensor");
    MHZ19CSerial.begin(9600);
    carbondioxide_sensor.begin(MHZ19CSerial);
    carbondioxide_sensor.autoCalibration();
#endif

#ifdef TSL2561_LUX_SENSOR
    Serial.println("Setting up Lux Sensor");
    // Initialize the SFE_TSL2561 library
    // You can pass nothing to light.begin() for the default I2C address (0x39),
    // or use one of the following presets if you have changed
    // the ADDR jumper on the board:
    // TSL2561_ADDR_0 address with '0' shorted on board (0x29)
    // TSL2561_ADDR   default address (0x39)
    // TSL2561_ADDR_1 address with '1' shorted on board (0x49)
    light_sensor.begin();
    // The light sensor has a default integration time of 402ms,
    // and a default gain of low (1X).
    // If you would like to change either of these, you can
    // do so using the setTiming() command.
    // If gain = false (0), device is set to low gain (1X)
    // If gain = high (1), device is set to high gain (16X)
    gain = 0;
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop to perform your own integration
    unsigned char time = 2;
    // setTiming() will set the third parameter (ms) to the requested integration time in ms (this will be useful later):
    Serial.println("Set timing...");
    light_sensor.setTiming(gain, time, ms);
    Serial.println("Powerup..."); // To start taking measurements, power up the sensor
    light_sensor.setPowerUp();
#endif

#ifdef PMS_DUST_SENSOR
    PMSerial2.begin(9600);
#endif
}

#ifdef HTU21D_TEMP_HUMIDITY_SENSOR
String collect_temperature_and_humidity_values(void)
{
    Serial.println("Reading HTU21D");
    uint8_t humd = temperature_and_humidity_sensor.readHumidity();
    uint8_t temp = temperature_and_humidity_sensor.readTemperature();
    Serial.println("Temperature: " + (String)temp + "C");
    Serial.println("Humidity: " + (String)humd + "%");
    return (",T:" + (String)temp + ",H:" + (String)humd);
}
#endif

#ifdef MHZ19C_CO2_SENSOR
String collect_co2_values(void)
{
    Serial.println("Reading MHZ19C");
    uint16_t CO2 = carbondioxide_sensor.getCO2();
    Serial.println("CO2: " + (String)CO2 + "ppm");
    return (",C:" + (String)CO2);
}
#endif

#ifdef TSL2561_LUX_SENSOR
String collect_lux_values(void)
{
    Serial.println("Reading TSL2561");
    double lux;
    unsigned int data0, data1;
    if (light_sensor.getData(data0, data1)) // getData() returned true, communication was successful
    {
        Serial.print("data0: ");
        Serial.print(data0);
        Serial.print(" data1: ");
        Serial.println(data1);
        boolean good;                                            // True if neither sensor is saturated
        good = light_sensor.getLux(gain, ms, data0, data1, lux); // Perform lux calculation:
        Serial.print("Lux: " + (String)lux);                     // Print out the results:
        if (good)
            Serial.println(" | GOOD");
        else
            Serial.println(" | BAD");
    }
    return (",L:" + (String)lux);
}
#endif

#ifdef PMS_DUST_SENSOR
void collect_dust_quality_data(void)
{
    memset(pm_ae_1_0, 0, VALUE_LENGTH);
    memset(pm_ae_2_5, 0, VALUE_LENGTH);
    memset(pm_ae_10_0, 0, VALUE_LENGTH);
    uint32_t timer = millis();
    while (!pms.read(data) && ((millis() - timer) < 2000))
        if (digitalRead(INT_PIN) == LOW)
            break;
    dtostrf(data.PM_AE_UG_1_0, MINIMUM_WIDTH, NUM_AFTER_DECIMAL, pm_ae_1_0);
    dtostrf(data.PM_AE_UG_2_5, MINIMUM_WIDTH, NUM_AFTER_DECIMAL, pm_ae_2_5);
    dtostrf(data.PM_AE_UG_10_0, MINIMUM_WIDTH, NUM_AFTER_DECIMAL, pm_ae_10_0);
}
#endif

#endif
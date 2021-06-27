#include <Arduino.h>

// DS18B20 temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// SSD 1306 0.9 128x32 OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "DebugHelper.h"

// DS18B20 temperature sensor
#define ONE_WIRE_BUS D5
#define SSR_PIN D6

// SSD 1306 0.9 128x32 OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// DS18B20 temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress sensorAddress;

// SSD 1306 0.9 128x32 OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// function to print the temperature for a device
// void printTemperature(DeviceAddress deviceAddress)
// {
//   float tempC = sensor.getTempC(deviceAddress);
//   if (tempC == DEVICE_DISCONNECTED_C)
//   {
//     Serial.println("Error: Could not read temperature data");
//     return;
//   }
//   Serial.print("Temp C: ");
//   Serial.print(tempC);
//   Serial.print(" Temp F: ");
//   Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
// }

unsigned long lastRequestTimeInMs = 0L;
const unsigned long requestIntervalInMs = 1L * 60L * 1000L;  
const float minTemperature = -18.00F;
const float maxTemperature = -15.00F;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  DEBUG_PRINT(F("Sensor address"));
  DEBUG_PRINT_APPEND(F(" = "));

  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) {
      DEBUG_PRINT_APPEND(F("0"));
    }
    DEBUG_PRINT_APPEND_HEX(deviceAddress[i]);
    DEBUG_PRINT_APPEND(" ");
  }
}

void switchRelayOn() {
  DEBUG_PRINTLN(F("Switching relay on"))
}

void switchRelayOff() {
  DEBUG_PRINTLN(F("Switching relay off"))
}

boolean isIntervalElapsed(unsigned long startTimeInMs, unsigned long currentTimeInMs, unsigned long intervalLengthInMs) {
  return intervalLengthInMs <= currentTimeInMs - startTimeInMs;
}

boolean isFreezerToWarm() {
  return sensor.getTempC(sensorAddress) > maxTemperature;
}

boolean isFreezerToCold() {
  return sensor.getTempC(sensorAddress) < minTemperature;
}

void initializeSensor() {
  // Initialize sensor
  sensor.begin();
  
  DEBUG_EMPTY_ROW
  DEBUG_EMPTY_ROW

  DEBUG_PRINTLN_KEY_VALUE(F("Number of devices"), sensor.getDeviceCount())

  if (!sensor.getAddress(sensorAddress, 0))
  {
    DEBUG_PRINTLN(F("Unable to find address for Device 0"));
  }

  printAddress(sensorAddress);

  sensor.setResolution(sensorAddress, 9);
}

void setup(void)
{
  // Initialize debug port
  DEBUG_BEGIN(115200)

  // Initialize display

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    DEBUG_PRINTLN(F("SSD1306 allocation failed"))
  }

  // Initialize temperature sensor
  initializeSensor();
}

void loop(void)
{
  unsigned long currentTime = millis();

  if (!isIntervalElapsed(lastRequestTimeInMs, currentTime, requestIntervalInMs))
  {
    return;
  }

  lastRequestTimeInMs = currentTime;
  sensor.requestTemperatures();

  if (isFreezerToWarm()) {
    switchRelayOn();
    DEBUG_PRINTLN_KEY_VALUE(F("Current temperature"), sensor.getTempC(sensorAddress))
    return;
  }

  if (isFreezerToCold()) {
    switchRelayOff();
    DEBUG_PRINTLN_KEY_VALUE(F("Current temperature"), sensor.getTempC(sensorAddress))
  }
}

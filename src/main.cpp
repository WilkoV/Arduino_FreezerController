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
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// DS18B20 temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress sensorAddress;

// SSD 1306 0.9 128x32 OLED
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define logic parameters
const unsigned long requestIntervalInMs = 1L * 60L * 1000L;
const float minTemperature = -20.00F;
const float maxTemperature = -18.00F;

unsigned long lastRequestTimeInMs = 0L;
boolean fridgeIsOn = false;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  DEBUG_PRINT(F("Sensor address"));
  DEBUG_PRINT_APPEND(F(" = "));

  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
    {
      DEBUG_PRINT_APPEND(F("0"));
    }
    DEBUG_PRINT_APPEND_HEX(deviceAddress[i]);
    DEBUG_PRINT_APPEND(" ");
  }
}

void switchRelayOn(u_int8_t pin)
{
  digitalWrite(pin, HIGH);
  fridgeIsOn = true;
  DEBUG_PRINTLN(F("Switching relay on"));
}

void switchRelayOff(u_int8_t pin)
{
  digitalWrite(pin, LOW);
  fridgeIsOn = false;
  DEBUG_PRINTLN(F("Switching relay off"))
}

boolean isIntervalElapsed(unsigned long startTimeInMs, unsigned long currentTimeInMs, unsigned long intervalLengthInMs)
{
  return intervalLengthInMs <= currentTimeInMs - startTimeInMs;
}

boolean isFreezerToWarm(float currentTemperature)
{
  return currentTemperature > maxTemperature;
}

boolean isFreezerToCold(float currentTemperature)
{
  return currentTemperature < minTemperature;
}

void initializeSensor(void)
{
  // Initialize sensor
  DEBUG_PRINTLN(F("Initializing sensor"))
  sensor.begin();

  // Scroll away from startup garbage
  DEBUG_PRINTLN_KEY_VALUE(F("Number of devices"), sensor.getDeviceCount())

  if (!sensor.getAddress(sensorAddress, 0))
  {
    DEBUG_PRINTLN(F("Unable to find address for Device 0"));
  }

  printAddress(sensorAddress);

  sensor.setResolution(sensorAddress, 9);
}

void initializeDisplay(void)
{
  DEBUG_PRINTLN(F("Initializing oled"))

  // SSD1306_SWITCHCAPVCC = generate oled voltage from 3.3V internally
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    DEBUG_PRINTLN(F("SSD1306 allocation failed"))
  }

  // clear oled
  oled.clearDisplay();
  oled.display();

  oled.setTextSize(2); // Draw 2X-scale text
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);

  oled.printf("Starting\n%d %d", (int)maxTemperature, (int)minTemperature);
 
  oled.display();
}

void showStatus(float currentTemperature, boolean relayIsOn)
{
  DEBUG_PRINTLN_KEY_VALUE(F("Current temperature"), sensor.getTempC(sensorAddress))
  DEBUG_PRINTLN_KEY_VALUE(F("Relay is "), relayIsOn ? "on" : "off")

  oled.clearDisplay();

  oled.setTextSize(2); // Draw 2X-scale text
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);

  if (currentTemperature >= 0)
  {
    oled.printf("%d.%02d\n%s", (int)currentTemperature, (int)(currentTemperature * 100) % 100, relayIsOn ? "an" : "aus");
  }
  else
  {
    currentTemperature = currentTemperature * -1;
    oled.printf("-%d.%02d\n%s", (int)currentTemperature, (int)(currentTemperature * 100) % 100, relayIsOn ? "an" : "aus");
  }
  
  oled.display(); // Show initial text
}

void setup(void)
{
  // Initialize debug port
  DEBUG_BEGIN(115200)
  DEBUG_EMPTY_ROW
  DEBUG_EMPTY_ROW

  delay(1000);

  // Initialize relay
  pinMode(SSR_PIN, OUTPUT);
  delay(1000);
  switchRelayOn(SSR_PIN);

  // Initialize oled
  initializeDisplay();

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
  float currentTemperature = sensor.getTempC(sensorAddress);

  showStatus(currentTemperature, fridgeIsOn);

  if (isFreezerToWarm(currentTemperature))
  {
    switchRelayOn(SSR_PIN);
    showStatus(currentTemperature, fridgeIsOn);
    return;
  }

  if (isFreezerToCold(currentTemperature))
  {
    switchRelayOff(SSR_PIN);
    showStatus(currentTemperature, fridgeIsOn);
  }
}

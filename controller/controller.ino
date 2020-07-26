#include <OneWire.h>
#include <DS18B20.h>

#include "thingProperties.h"
#include "motor_timers.hpp"

// -------------------- Define Pins ---------------------
#define PUMP_1_RELAY_PIN 0
#define PUMP_2_RELAY_PIN 1
#define FEEDER_RELAY_PIN 2
#define TEMP_1_PIN       4
#define TEMP_2_PIN       5
#define WATER_SENSOR_1_PIN 16
#define WATER_SENSOR_2_PIN 17

// ------------------- Define Times --------------------------
// 60,000 ms in a minute
// 3,600,000 ms in an hour
// 86,400,000 ms in a day
#define FEEDER_PERIOD_MS    10000
#define FEEDER_DURATION_MS  1000
#define PUMP_1_PERIOD_MS    20000
#define PUMP_1_DURATION_MS  10000
#define PUMP_2_DURATION_MS  10000


// initialize serial interface for temperature sensors
OneWire oneWire_1(TEMP_1_PIN);
OneWire oneWire_2(TEMP_2_PIN);
DS18B20 temp_sens_1(&oneWire_1);
DS18B20 temp_sens_2(&oneWire_2);

// set up timers for time-triggered events
TimeTrigger feeder_timer(FEEDER_PERIOD_MS, FEEDER_DURATION_MS);
TimeTrigger pump_1_timer(PUMP_1_PERIOD_MS, PUMP_1_DURATION_MS);
Timer       pump_2_timer(PUMP_2_DURATION_MS);

// set up system
void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Set pin modes
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUMP_1_RELAY_PIN,   OUTPUT);
  pinMode(PUMP_2_RELAY_PIN,   OUTPUT);
  pinMode(FEEDER_RELAY_PIN,   OUTPUT);
  pinMode(TEMP_1_PIN,         INPUT);
  pinMode(TEMP_2_PIN,         INPUT);
  pinMode(WATER_SENSOR_1_PIN, INPUT);
  pinMode(WATER_SENSOR_2_PIN, INPUT);

  // set up temperature sensors
  temp_sens_1.begin();
  temp_sens_2.begin();
  
}

// main loop
void loop() {
  // update cloud interface
  ArduinoCloud.update();

  // read water sensors
  int water_sensor_1_voltage = analogRead(WATER_SENSOR_1_PIN);
  water_sensor_1 = water_sensor_1_voltage * 100.0 / 1024.0;

  int water_sensor_2_voltage = analogRead(WATER_SENSOR_2_PIN);
  float water_sensor_2 = water_sensor_2_voltage * 100.0 / 1024.0;

  // read tempurature sensors
  temp_sens_1.requestTemperatures();
  while (!temp_sens_1.isConversionComplete());  // wait until sensor is ready
  temp_1 = temp_sens_1.getTempC();

  temp_sens_2.requestTemperatures();
  while (!temp_sens_2.isConversionComplete());  // wait until sensor is ready
  temp_2 = temp_sens_2.getTempC();

  Serial.println("water sensor 1: " + String(water_sensor_1));
  Serial.println("water sensor 2: " + String(water_sensor_2));
  if(water_sensor_2 > 40)
  {
    pump_2_timer.start();
  }
  
  digitalWrite(FEEDER_RELAY_PIN, feeder_timer.isActive());
  digitalWrite(PUMP_1_RELAY_PIN, pump_1_timer.isActive());
  digitalWrite(PUMP_2_RELAY_PIN, pump_2_timer.isActive());
  
  delay(10);
}



void onPump2Change() {
  digitalWrite(PUMP_2_RELAY_PIN, pump_2);
  digitalWrite(FEEDER_RELAY_PIN, pump_2);
}


void onPump1Change() {
  digitalWrite(PUMP_1_RELAY_PIN, pump_1);
}

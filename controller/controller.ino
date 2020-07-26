#include <OneWire.h>
#include <DS18B20.h>

/* 
  Sketch generated by the Arduino IoT Cloud Thing "apc"
  https://create.arduino.cc/cloud/things/c507c447-e781-4106-ab41-6e104ce1e233 

  Arduino IoT Cloud Properties description

  The following variables are automatically generated and updated when changes are made to the Thing properties

  float temp_1;
  float temp_2;
  float water_sensor;
  bool pump_1;
  bool pump_2;

  Properties which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"

#define PUMP_1_RELAY_PIN 0
#define PUMP_2_RELAY_PIN 1
#define FEEDER_RELAY_PIN 2
#define TEMP_1_PIN       4
#define TEMP_2_PIN       5
#define WATER_SENSOR_1_PIN 16
#define WATER_SENSOR_2_PIN 17



OneWire oneWire_1(TEMP_1_PIN);
OneWire oneWire_2(TEMP_2_PIN);
DS18B20 temp_sens_1(&oneWire_1);
DS18B20 temp_sens_2(&oneWire_2);

enum MotorState
{
  WAITING,
  RUNNING,
  
};

class Timer
{
public:
  Timer(unsigned long timer_duration_ms) : duration_ms(timer_duration_ms),
                                           end_time_ms(0)
  {
    
  }

  void start()
  {
    end_time_ms = millis() + duration_ms;
  }

  bool isActive()
  {
    return millis() < end_time_ms;
  }
private:
  unsigned long duration_ms;
  unsigned long end_time_ms;
};

class TimeTrigger
{
public:

  TimeTrigger(unsigned long timer_period_ms, unsigned long timer_run_duration_ms) : period_ms(timer_period_ms), 
                                                                                    run_duration_ms(timer_run_duration_ms),
                                                                                    active(false)
  {
    unsigned long now = millis();
    next_start_time_ms = now + timer_period_ms;
    next_stop_time_ms  = now + timer_period_ms + run_duration_ms;
    
  }
  
  bool isActive()
  {
    updateState();
    return active;
  }

  unsigned long getCountdownTime_ms()
  {
    return next_start_time_ms - millis();
  }

private:
  void updateState()
  {
    unsigned long now = millis();
    if (active)
    {
      // see if run duration is complete
      if (now > next_stop_time_ms)
      {
        active = false;
      }
    }
    else // not active
    {
      // see if it's time to activate
      if (now > next_start_time_ms)
      {
        active = true;
        next_start_time_ms = now + period_ms;
        next_stop_time_ms  = now + run_duration_ms;
      }
    }
  }

  bool          active;               // indicates if trigger is active/on
  unsigned long period_ms;            // how often is this triggered?
  unsigned long run_duration_ms;      // how long should the motor run?'
  unsigned long next_start_time_ms;   // when should we go active again?
  unsigned long next_stop_time_ms;    // when should we go inactive again?
};

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

void loop() {
  // update cloud interface
  ArduinoCloud.update();

  // read water sensor
  int water_sensor_voltage = analogRead(WATER_SENSOR_1_PIN);
  water_sensor = water_sensor_voltage * 100.0 / 1024.0;

  // read tempurature sensors
  temp_sens_1.requestTemperatures();
  while (!temp_sens_1.isConversionComplete());  // wait until sensor is ready
  temp_1 = temp_sens_1.getTempC();

  temp_sens_2.requestTemperatures();
  while (!temp_sens_2.isConversionComplete());  // wait until sensor is ready
  temp_2 = temp_sens_2.getTempC();
  
  delay(1000);
}



void onPump2Change() {
  digitalWrite(PUMP_2_RELAY_PIN, pump_2);
  digitalWrite(FEEDER_RELAY_PIN, pump_2);
}


void onPump1Change() {
  digitalWrite(PUMP_1_RELAY_PIN, pump_1);
}

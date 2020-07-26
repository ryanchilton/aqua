#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

#include "arduino_secrets.h"

const char THING_ID[] = "c507c447-e781-4106-ab41-6e104ce1e233";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)

void onPump1Change();
void onPump2Change();

float temp_1;
float temp_2;
float water_sensor_1;
bool pump_1;
bool pump_2;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temp_1, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(temp_2, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(water_sensor_1, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(pump_1, READWRITE, ON_CHANGE, onPump1Change);
  ArduinoCloud.addProperty(pump_2, READWRITE, ON_CHANGE, onPump2Change);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

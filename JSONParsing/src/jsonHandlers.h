#ifndef JSON_HANDLERS_H
#define JSON_HANDLERS_H

#include "Particle.h"

// Structure to hold parsed sensor register JSON data
struct SensorRegister {
    String action;
    String sensorName;
    String onSensorClosed_action;
    int onSensorClosed_delayMin;
    String onSensorClosed_alertWhenActiveWindowStarts;
    String onSensorOpen_action;
    int onSensorOpen_delayMin;
    String onSensorOpen_alertWhenActiveWindowStarts;
    int alertWindow_timeStart;
    int alertWindow_durationHours;
    String alertParams_pushoverSound;
    String alertParams_pushoverPriority;
};

// Hub config structure
struct HubConfig {
    String hubName;
    String timeLocal;
    int loRaNetwork;
    String ownerEmail;
    int uptimeHours;
    String alertMethod_type;
    String alertMethod_email;
};

#define MAX_SENSOR_REGISTERS 5

// Test strings
extern const String testRegisterString;
extern const String testHubConfigString;

// Functions to populate structs from JSON
void populateSensorRegister(JSONValue obj, SensorRegister& reg);
void populateHubConfig(JSONValue obj, HubConfig& cfg);

// Functions to serialize structs to JSON
String sensorRegisterToJson(const SensorRegister& reg);
String hubConfigToJson(const HubConfig& cfg);
String hubStatusToJson(const HubConfig& cfg, const SensorRegister* regs, int count);

#endif // JSON_HANDLERS_H

/*
 * jsonconversiontests.ino:  Test of JSON parsing and writing using Particle built-in json library
 * Author: Bob Glicksman
 * Date: 11/06/26
 *
 * Version 0.1, 11/05/26.  Got basic parsing working.  It detects bad tags.  It does not detect
 *      improperly formatted json object strings.  Still need to work on json object writing.
 * Version 0.2, 11/06/26.  Added and tested checking for invalid json string formatting.  Still
 *      need to work on json object writing.
 * Version 0.3, 11/06/26.  Reads test string from testRegister.txt format (sensorSlot,JSON),
 *      extracts sensorSlot, deserializes the JSON, and debug-prints all values including
 *      nested objects and arrays.
 * Version 0.4, 11/06/26.  Added detailed debug printing of all JSON values, including nested objects and arrays.
 * Version 0.5, 11/09/26.  Moved JSON handling code to jsonHandlers.h.
 *
 * (c) 2027, Bob Glicksman, Jim Schrempp, Team Practical Projects.  All rights reserved.
 *
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "jsonHandlers.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Global variables
int sensorSlot = 0;
SensorRegister sensorRegs[MAX_SENSOR_REGISTERS];
HubConfig hubCfg;

// setup() runs once, when the device is first turned on
void setup() {
    // Wait for USB serial to be ready before printing
    waitUntil(Serial.isConnected);
    Log.info("Serial is connected");

    // Step 1: Read the string from testRegister.txt (hardcoded for testing)
    Serial.println("=== Test register string ===");
    Serial.println(testRegisterString);
    Serial.println();

    // Step 2: Extract the sensorSlot from before the first comma
    int commaIndex = testRegisterString.indexOf(',');
    if (commaIndex <= 0) {
        Serial.println("ERROR: No comma found in input string");
        return;
    }

    String slotStr = testRegisterString.substring(0, commaIndex);
    slotStr.trim();
    sensorSlot = slotStr.toInt();
    Serial.print("Sensor slot: ");
    Serial.println(sensorSlot);
    Serial.println();

    // Step 3: Deserialize the JSON that comes after the comma
    String jsonStr = testRegisterString.substring(commaIndex + 1);
    jsonStr.trim();
    Serial.println("=== JSON Input String ===");
    Serial.println(jsonStr);
    Serial.println();

    {
        JSONValue outerObj = JSONValue::parseCopy(jsonStr);

        if (!outerObj.isValid()) {
            Serial.println("*** INVALID JSON ***");
            return;
        }

        // Step 4: Populate the global struct from the JSON
        int idx = (sensorSlot >= 0 && sensorSlot < MAX_SENSOR_REGISTERS) ? sensorSlot : 0;
        populateSensorRegister(outerObj, sensorRegs[idx]);
    }
    // outerObj released here — scope ended

    // Step 5: Print values from the global struct
    Serial.println();
    Serial.print("=== Values from struct [");
    Serial.print(sensorSlot);
    Serial.println("] ===");

    SensorRegister& reg = sensorRegs[(sensorSlot >= 0 && sensorSlot < MAX_SENSOR_REGISTERS) ? sensorSlot : 0];

    Serial.print("action: ");
    Serial.println(reg.action);
    Serial.print("sensorName: ");
    Serial.println(reg.sensorName);

    Serial.println("onSensorClosed:");
    Serial.print("  action: ");
    Serial.println(reg.onSensorClosed_action);
    Serial.print("  delayMin: ");
    Serial.println(reg.onSensorClosed_delayMin);
    Serial.print("  alertWhenActiveWindowStarts: ");
    Serial.println(reg.onSensorClosed_alertWhenActiveWindowStarts);

    Serial.println("onSensorOpen:");
    Serial.print("  action: ");
    Serial.println(reg.onSensorOpen_action);
    Serial.print("  delayMin: ");
    Serial.println(reg.onSensorOpen_delayMin);
    Serial.print("  alertWhenActiveWindowStarts: ");
    Serial.println(reg.onSensorOpen_alertWhenActiveWindowStarts);

    Serial.println("alertWindow:");
    Serial.print("  timeStart: ");
    Serial.println(reg.alertWindow_timeStart);
    Serial.print("  durationHours: ");
    Serial.println(reg.alertWindow_durationHours);

    Serial.println("alertParams:");
    Serial.print("  pushoverSound: ");
    Serial.println(reg.alertParams_pushoverSound);
    Serial.print("  pushoverPriority: ");
    Serial.println(reg.alertParams_pushoverPriority);

    Serial.println("=== Parsing complete ===");

    // Step 6: Re-serialize the struct back to JSON
    Serial.println();
    Serial.println("=== JSON from struct ===");
    Serial.println(sensorRegisterToJson(sensorRegs[(sensorSlot >= 0 && sensorSlot < MAX_SENSOR_REGISTERS) ? sensorSlot : 0]));

    // --- Hub config test ---
    Serial.println();
    Serial.println("=== Hub config JSON ===");
    Serial.println(testHubConfigString);

    {
        JSONValue hubObj = JSONValue::parseCopy(testHubConfigString);
        if (!hubObj.isValid()) {
            Serial.println("*** INVALID HUB JSON ***");
            return;
        }
        populateHubConfig(hubObj, hubCfg);
    }

    // Print parsed hub config
    Serial.println();
    Serial.println("=== Hub config values ===");
    Serial.print("hubName: ");
    Serial.println(hubCfg.hubName);
    Serial.print("timeLocal: ");
    Serial.println(hubCfg.timeLocal);
    Serial.print("LoRaNetwork: ");
    Serial.println(hubCfg.loRaNetwork);
    Serial.print("ownerEmail: ");
    Serial.println(hubCfg.ownerEmail);
    Serial.print("uptimeHours: ");
    Serial.println(hubCfg.uptimeHours);
    Serial.println("alertMethod:");
    Serial.print("  type: ");
    Serial.println(hubCfg.alertMethod_type);
    Serial.print("  email: ");
    Serial.println(hubCfg.alertMethod_email);

    // Re-serialize hub config
    Serial.println();
    Serial.println("=== Hub config from struct ===");
    Serial.println(hubConfigToJson(hubCfg));

    // --- Hub status test ---
    Serial.println();
    Serial.println("=== Hub status JSON ===");
    Serial.println(hubStatusToJson(hubCfg, sensorRegs, MAX_SENSOR_REGISTERS));

}   // end of setup()

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    // All work done in setup()
}  // end of loop()




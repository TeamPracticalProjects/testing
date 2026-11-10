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
 *
 * (c) 2027, Bob Glicksman, Jim Schrempp, Team Practical Projects.  All rights reserved.
 *
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Test string matching the content of testRegister.txt
// Format: sensorSlot,JSON
const String testRegisterString =
  "5,\n"
  "{\n"
  "  \"action\": \"update\",\n"
  "  \"sensorName\": \"Back gate\",\n"
  "  \"onSensorClosed\": {\n"
  "      \"action\": \"ActionNone\",\n"
  "      \"delayMin\": 0,\n"
  "      \"alertWhenActiveWindowStarts\": \"false\"\n"
  "   },\n"
  "  \"onSensorOpen\": {\n"
  "      \"action\": \"ActionDelay\",\n"
  "      \"delayMin\": 30,\n"
  "      \"alertWhenActiveWindowStarts\": \"true\"\n"
  "   },\n"
  "  \"alertWindow\": {\n"
  "      \"timeStart\": 18,\n"
  "      \"durationHours\": 12\n"
  "   },\n"
  "  \"alertParams\": {\n"
  "      \"pushoverSound\": \"7\",\n"
  "      \"pushoverPriority\": \"2\"\n"
  "  }\n"
  "}";

// Structure to hold parsed JSON data
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

// Global variables
int sensorSlot = 0;
SensorRegister sensorReg;

// Helper: populate the SensorRegister from the top-level JSON object
void populateSensorRegister(JSONValue obj, SensorRegister& reg) {
    JSONObjectIterator topIter(obj);
    while (topIter.next()) {
        JSONString name = topIter.name();
        JSONValue val = topIter.value();

        if (name == "action") {
            reg.action = (const char*)val.toString();
        } else if (name == "sensorName") {
            reg.sensorName = (const char*)val.toString();
        } else if (name == "onSensorClosed") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "action") {
                    reg.onSensorClosed_action = (const char*)iter.value().toString();
                } else if (n == "delayMin") {
                    reg.onSensorClosed_delayMin = iter.value().toInt();
                } else if (n == "alertWhenActiveWindowStarts") {
                    reg.onSensorClosed_alertWhenActiveWindowStarts = (const char*)iter.value().toString();
                }
            }
        } else if (name == "onSensorOpen") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "action") {
                    reg.onSensorOpen_action = (const char*)iter.value().toString();
                } else if (n == "delayMin") {
                    reg.onSensorOpen_delayMin = iter.value().toInt();
                } else if (n == "alertWhenActiveWindowStarts") {
                    reg.onSensorOpen_alertWhenActiveWindowStarts = (const char*)iter.value().toString();
                }
            }
        } else if (name == "alertWindow") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "timeStart") {
                    reg.alertWindow_timeStart = iter.value().toInt();
                } else if (n == "durationHours") {
                    reg.alertWindow_durationHours = iter.value().toInt();
                }
            }
        } else if (name == "alertParams") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "pushoverSound") {
                    reg.alertParams_pushoverSound = (const char*)iter.value().toString();
                } else if (n == "pushoverPriority") {
                    reg.alertParams_pushoverPriority = (const char*)iter.value().toString();
                }
            }
        }
    }
}

// Helper: create a JSON string from a SensorRegister
String sensorRegisterToJson(const SensorRegister& reg) {
    char buf[1024];
    JSONBufferWriter writer(buf, sizeof(buf) - 1);

    writer.beginObject();
        writer.name("action").value(reg.action);
        writer.name("sensorName").value(reg.sensorName);

        writer.name("onSensorClosed").beginObject();
            writer.name("action").value(reg.onSensorClosed_action);
            writer.name("delayMin").value(reg.onSensorClosed_delayMin);
            writer.name("alertWhenActiveWindowStarts").value(reg.onSensorClosed_alertWhenActiveWindowStarts);
        writer.endObject();

        writer.name("onSensorOpen").beginObject();
            writer.name("action").value(reg.onSensorOpen_action);
            writer.name("delayMin").value(reg.onSensorOpen_delayMin);
            writer.name("alertWhenActiveWindowStarts").value(reg.onSensorOpen_alertWhenActiveWindowStarts);
        writer.endObject();

        writer.name("alertWindow").beginObject();
            writer.name("timeStart").value(reg.alertWindow_timeStart);
            writer.name("durationHours").value(reg.alertWindow_durationHours);
        writer.endObject();

        writer.name("alertParams").beginObject();
            writer.name("pushoverSound").value(reg.alertParams_pushoverSound);
            writer.name("pushoverPriority").value(reg.alertParams_pushoverPriority);
        writer.endObject();
    writer.endObject();

    buf[writer.dataSize()] = '\0';
    return String(buf);
}

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
        populateSensorRegister(outerObj, sensorReg);
    }
    // outerObj released here — scope ended

    // Step 5: Print values from the global struct
    Serial.println();
    Serial.println("=== Values from struct ===");

    Serial.print("action: ");
    Serial.println(sensorReg.action);
    Serial.print("sensorName: ");
    Serial.println(sensorReg.sensorName);

    Serial.println("onSensorClosed:");
    Serial.print("  action: ");
    Serial.println(sensorReg.onSensorClosed_action);
    Serial.print("  delayMin: ");
    Serial.println(sensorReg.onSensorClosed_delayMin);
    Serial.print("  alertWhenActiveWindowStarts: ");
    Serial.println(sensorReg.onSensorClosed_alertWhenActiveWindowStarts);

    Serial.println("onSensorOpen:");
    Serial.print("  action: ");
    Serial.println(sensorReg.onSensorOpen_action);
    Serial.print("  delayMin: ");
    Serial.println(sensorReg.onSensorOpen_delayMin);
    Serial.print("  alertWhenActiveWindowStarts: ");
    Serial.println(sensorReg.onSensorOpen_alertWhenActiveWindowStarts);

    Serial.println("alertWindow:");
    Serial.print("  timeStart: ");
    Serial.println(sensorReg.alertWindow_timeStart);
    Serial.print("  durationHours: ");
    Serial.println(sensorReg.alertWindow_durationHours);

    Serial.println("alertParams:");
    Serial.print("  pushoverSound: ");
    Serial.println(sensorReg.alertParams_pushoverSound);
    Serial.print("  pushoverPriority: ");
    Serial.println(sensorReg.alertParams_pushoverPriority);

    Serial.println("=== Parsing complete ===");

    // Step 6: Re-serialize the struct back to JSON
    Serial.println();
    Serial.println("=== JSON from struct ===");
    Serial.println(sensorRegisterToJson(sensorReg));

}   // end of setup()

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    // All work done in setup()
}  // end of loop()



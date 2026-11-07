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

// Global variables
int sensorSlot = 0;

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

    JSONValue outerObj = JSONValue::parseCopy(jsonStr);

    if (!outerObj.isValid()) {
        Serial.println("*** INVALID JSON ***");
        return;
    }

    // Step 5: Access all values directly
    Serial.println();
    Serial.println("=== All values Parsed ===");

    // Top-level scalars
    JSONObjectIterator topIter(outerObj);
    while (topIter.next()) {
        JSONString name = topIter.name();
        JSONValue val = topIter.value();

        if (name == "action") {
            Serial.print("action: ");
            Serial.println((const char*)val.toString());
        } else if (name == "sensorName") {
            Serial.print("sensorName: ");
            Serial.println((const char*)val.toString());
        } else if (name == "onSensorClosed") {
            Serial.println("onSensorClosed:");
            JSONObjectIterator entryIter(val);
            while (entryIter.next()) {
                JSONString ename = entryIter.name();
                JSONValue eval = entryIter.value();
                if (ename == "action") {
                    Serial.print("  action: ");
                    Serial.println((const char*)eval.toString());
                } else if (ename == "delayMin") {
                    Serial.print("  delayMin: ");
                    Serial.println(eval.toInt());
                } else if (ename == "alertWhenActiveWindowStarts") {
                    Serial.print("  alertWhenActiveWindowStarts: ");
                    Serial.println((const char*)eval.toString());
                }
            }
        } else if (name == "onSensorOpen") {
            Serial.println("onSensorOpen:");
            JSONObjectIterator entryIter(val);
            while (entryIter.next()) {
                JSONString ename = entryIter.name();
                JSONValue eval = entryIter.value();
                if (ename == "action") {
                    Serial.print("  action: ");
                    Serial.println((const char*)eval.toString());
                } else if (ename == "delayMin") {
                    Serial.print("  delayMin: ");
                    Serial.println(eval.toInt());
                } else if (ename == "alertWhenActiveWindowStarts") {
                    Serial.print("  alertWhenActiveWindowStarts: ");
                    Serial.println((const char*)eval.toString());
                }
            }
        } else if (name == "alertWindow") {
            Serial.println("alertWindow:");
            JSONObjectIterator entryIter(val);
            while (entryIter.next()) {
                JSONString ename = entryIter.name();
                JSONValue eval = entryIter.value();
                if (ename == "timeStart") {
                    Serial.print("  timeStart: ");
                    Serial.println(eval.toInt());
                } else if (ename == "durationHours") {
                    Serial.print("  durationHours: ");
                    Serial.println(eval.toInt());
                }
            }
        } else if (name == "alertParams") {
            Serial.println("alertParams:");
            JSONObjectIterator entryIter(val);
            while (entryIter.next()) {
                JSONString ename = entryIter.name();
                JSONValue eval = entryIter.value();
                if (ename == "pushoverSound") {
                    Serial.print("  pushoverSound: ");
                    Serial.println((const char*)eval.toString());
                } else if (ename == "pushoverPriority") {
                    Serial.print("  pushoverPriority: ");
                    Serial.println((const char*)eval.toString());
                }
            }
        }
    }

    Serial.println("=== Parsing complete ===");

}   // end of setup()

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    // All work done in setup()
}  // end of loop()



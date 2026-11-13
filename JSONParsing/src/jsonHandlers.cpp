#include "jsonHandlers.h"

// Test string matching the content of testRegister.txt
// Format: sensorSlot,JSON
const String testRegisterString =
  "5,\n"
  "{\n"
  "  \"action\": \"update\",\n"
  "  \"name\": \"Back gate\",\n"
  "  \"onSensorClosed\": {\n"
  "      \"action\": \"ActionNone\",\n"
  "      \"delayMin\": 0,\n"
  "      \"AWAWS\": \"false\"\n"
  "   },\n"
  "  \"onSensorOpen\": {\n"
  "      \"action\": \"ActionDelay\",\n"
  "      \"delayMin\": 30,\n"
  "      \"AWAWS\": \"true\"\n"
  "   },\n"
  "  \"alertWindow\": {\n"
  "      \"timeStart\": 18,\n"
  "      \"durH\": 12\n"
  "   },\n"
  "  \"alertParams\": {\n"
  "      \"pshSnd\": \"7\",\n"
  "      \"pshPri\": \"2\"\n"
  "  }\n"
  "}";

// Test string for hub config
const String testHubConfigString =
  "{\n"
  "  \"name\": \"Jim's hub\",\n"
  "  \"time\": \"20260319 15:16\",\n"
  "  \"network\": 12,\n"
  "  \"owner\": \"youremail@yourprovider.com\",\n"
  "  \"upHours\": 43000,\n"
  "  \"alertMethod\": {\n"
  "      \"type\": \"pushover\",\n"
  "      \"email\": \"xyz@pushover.com\"\n"
  "  }\n"
  "}";

// Populate a SensorRegister from a JSON object
void populateSensorRegister(JSONValue obj, SensorRegister& reg) {
    JSONObjectIterator topIter(obj);
    while (topIter.next()) {
        JSONString name = topIter.name();
        JSONValue val = topIter.value();

        if (name == "action") {
            reg.action = (const char*)val.toString();
        } else if (name == "name") {
            reg.sensorName = (const char*)val.toString();
        } else if (name == "onSensorClosed") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "action") {
                    reg.onSensorClosed_action = (const char*)iter.value().toString();
                } else if (n == "delayMin") {
                    reg.onSensorClosed_delayMin = iter.value().toInt();
                } else if (n == "AWAWS") {
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
                } else if (n == "AWAWS") {
                    reg.onSensorOpen_alertWhenActiveWindowStarts = (const char*)iter.value().toString();
                }
            }
        } else if (name == "alertWindow") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "timeStart") {
                    reg.alertWindow_timeStart = iter.value().toInt();
                } else if (n == "durH") {
                    reg.alertWindow_durationHours = iter.value().toInt();
                }
            }
        } else if (name == "alertParams") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "pshSnd") {
                    reg.alertParams_pushoverSound = (const char*)iter.value().toString();
                } else if (n == "pshPri") {
                    reg.alertParams_pushoverPriority = (const char*)iter.value().toString();
                }
            }
        }
    }
}

// Serialize a SensorRegister to JSON
String sensorRegisterToJson(const SensorRegister& reg) {
    char buf[1024];
    JSONBufferWriter writer(buf, sizeof(buf) - 1);

    writer.beginObject();
        writer.name("action").value(reg.action);
        writer.name("name").value(reg.sensorName);

        writer.name("onSensorClosed").beginObject();
            writer.name("action").value(reg.onSensorClosed_action);
            writer.name("delayMin").value(reg.onSensorClosed_delayMin);
            writer.name("AWAWS").value(reg.onSensorClosed_alertWhenActiveWindowStarts);
        writer.endObject();

        writer.name("onSensorOpen").beginObject();
            writer.name("action").value(reg.onSensorOpen_action);
            writer.name("delayMin").value(reg.onSensorOpen_delayMin);
            writer.name("AWAWS").value(reg.onSensorOpen_alertWhenActiveWindowStarts);
        writer.endObject();

        writer.name("alertWindow").beginObject();
            writer.name("timeStart").value(reg.alertWindow_timeStart);
            writer.name("durH").value(reg.alertWindow_durationHours);
        writer.endObject();

        writer.name("alertParams").beginObject();
            writer.name("pshSnd").value(reg.alertParams_pushoverSound);
            writer.name("pshPri").value(reg.alertParams_pushoverPriority);
        writer.endObject();
    writer.endObject();

    buf[writer.dataSize()] = '\0';
    return String(buf);
}

// Populate a HubConfig from a JSON object
void populateHubConfig(JSONValue obj, HubConfig& cfg) {
    JSONObjectIterator topIter(obj);
    while (topIter.next()) {
        JSONString name = topIter.name();
        JSONValue val = topIter.value();

        if (name == "name") {
            cfg.hubName = (const char*)val.toString();
        } else if (name == "time") {
            cfg.timeLocal = (const char*)val.toString();
        } else if (name == "network") {
            cfg.loRaNetwork = val.toInt();
        } else if (name == "owner") {
            cfg.ownerEmail = (const char*)val.toString();
        } else if (name == "upHours") {
            cfg.uptimeHours = val.toInt();
        } else if (name == "alertMethod") {
            JSONObjectIterator iter(val);
            while (iter.next()) {
                JSONString n = iter.name();
                if (n == "type") {
                    cfg.alertMethod_type = (const char*)iter.value().toString();
                } else if (n == "email") {
                    cfg.alertMethod_email = (const char*)iter.value().toString();
                }
            }
        }
    }
}

// Serialize a HubConfig to JSON
String hubConfigToJson(const HubConfig& cfg) {
    char buf[1024];
    JSONBufferWriter writer(buf, sizeof(buf) - 1);

    writer.beginObject();
        writer.name("name").value(cfg.hubName);
        writer.name("time").value(cfg.timeLocal);
        writer.name("network").value(cfg.loRaNetwork);
        writer.name("owner").value(cfg.ownerEmail);
        writer.name("upHours").value(cfg.uptimeHours);
        writer.name("alertMethod").beginObject();
            writer.name("type").value(cfg.alertMethod_type);
            writer.name("email").value(cfg.alertMethod_email);
        writer.endObject();
    writer.endObject();

    buf[writer.dataSize()] = '\0';
    return String(buf);
}

// Create a hub status JSON from HubConfig and SensorRegister array
String hubStatusToJson(const HubConfig& cfg, const SensorRegister* regs, int count) {
    char buf[2048];
    JSONBufferWriter writer(buf, sizeof(buf) - 1);

    writer.beginObject();
        writer.name("name").value(cfg.hubName);
        writer.name("time").value(cfg.timeLocal);
        writer.name("sensors").beginArray();
            for (int i = 0; i < count; i++) {
                if (regs[i].sensorName.length() == 0) continue;
                writer.beginObject();
                    writer.name("LoRa").value(0);
                    writer.name("name").value(regs[i].sensorName);
                    writer.name("slot").value(i);
                    writer.name("state").value("unknown");
                    writer.name("asOf").value("");
                writer.endObject();
            }
        writer.endArray();
    writer.endObject();

    buf[writer.dataSize()] = '\0';
    return String(buf);
}

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

namespace {
constexpr uint16_t DNS_PORT = 53;
const char* AP_SSID = "OnsiteCIO-Demo";
const char* AP_PASSWORD = "";
const char* STATE_FILE = "/state.json";

WebServer server(80);
DNSServer dns;

String readFile(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) return String();
  String content;
  while (file.available()) content += static_cast<char>(file.read());
  file.close();
  return content;
}

bool writeFile(const char* path, const String& data) {
  File file = LittleFS.open(path, "w");
  if (!file) return false;
  file.print(data);
  file.close();
  return true;
}

String defaultStateJson() {
  JsonDocument doc;
  doc["branding"]["companyName"] = "Colorado Onsite CIO";
  doc["branding"]["supportEmail"] = "support@example.com";
  doc["branding"]["supportPhone"] = "(555) 555-5555";
  doc["branding"]["themeColor"] = "#1f6feb";
  doc["branding"]["logoDataUrl"] = "";

  JsonArray checklist = doc["checklist"].to<JsonArray>();
  const char* defaults[] = {
      "Capture old device inventory",
      "Setup identity and email profile",
      "Transfer browser profile and bookmarks",
      "Verify line-of-business apps",
      "Validate backup and endpoint security"
  };
  for (const auto* item : defaults) {
    JsonObject obj = checklist.add<JsonObject>();
    obj["title"] = item;
    obj["done"] = false;
    obj["notes"] = "";
  }

  JsonArray softwareProfiles = doc["profiles"].to<JsonArray>();
  JsonObject smb = softwareProfiles.add<JsonObject>();
  smb["name"] = "Small business standard";
  smb["packages"] = "Office, Chrome, Zoom, RMM Agent";
  smb["checks"] = "Sign-in, update cycle, default app policy";

  JsonObject accounting = softwareProfiles.add<JsonObject>();
  accounting["name"] = "Accounting workstation";
  accounting["packages"] = "Office, Chrome, Zoom, VPN, Accounting LOB app";
  accounting["checks"] = "Printer map, line-of-business launch, MFA";

  JsonObject exec = softwareProfiles.add<JsonObject>();
  exec["name"] = "Executive travel laptop";
  exec["packages"] = "Office, Chrome, Teams, VPN, Password manager";
  exec["checks"] = "BitLocker status, sync status, remote access";

  JsonArray logs = doc["logs"].to<JsonArray>();
  JsonObject first = logs.add<JsonObject>();
  first["timestamp"] = "Boot";
  first["message"] = "Portal initialized";

  String out;
  serializeJson(doc, out);
  return out;
}

String getStateJson() {
  if (!LittleFS.exists(STATE_FILE)) {
    String init = defaultStateJson();
    writeFile(STATE_FILE, init);
    return init;
  }

  String data = readFile(STATE_FILE);
  if (data.isEmpty()) {
    data = defaultStateJson();
    writeFile(STATE_FILE, data);
  }
  return data;
}

bool mergeAndSave(const String& patchJson) {
  JsonDocument current;
  JsonDocument patch;
  if (deserializeJson(current, getStateJson()) != DeserializationError::Ok) return false;
  if (deserializeJson(patch, patchJson) != DeserializationError::Ok) return false;

  for (JsonPair kv : patch.as<JsonObject>()) {
    current[kv.key()] = kv.value();
  }

  String out;
  serializeJson(current, out);
  return writeFile(STATE_FILE, out);
}

void sendJson(int statusCode, const String& json) {
  server.sendHeader("Cache-Control", "no-store");
  server.send(statusCode, "application/json", json);
}

void appendLog(const String& message) {
  JsonDocument doc;
  if (deserializeJson(doc, getStateJson()) != DeserializationError::Ok) return;

  JsonArray logs = doc["logs"].to<JsonArray>();
  JsonObject entry = logs.add<JsonObject>();
  entry["timestamp"] = String(millis());
  entry["message"] = message;

  String out;
  serializeJson(doc, out);
  writeFile(STATE_FILE, out);
}

String csvEscape(const String& in) {
  String out = in;
  out.replace("\"", "\"\"");
  return "\"" + out + "\"";
}

void handleCaptivePortal() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

void mountRoutes() {
  server.on("/", HTTP_GET, []() {
    File f = LittleFS.open("/index.html", "r");
    if (!f) {
      server.send(500, "text/plain", "Missing /index.html in LittleFS.");
      return;
    }
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/api/state", HTTP_GET, []() {
    sendJson(200, getStateJson());
  });

  server.on("/api/state", HTTP_POST, []() {
    String body = server.arg("plain");
    if (body.isEmpty()) {
      sendJson(400, "{\"error\":\"Missing payload\"}");
      return;
    }

    if (!mergeAndSave(body)) {
      sendJson(400, "{\"error\":\"Invalid JSON payload\"}");
      return;
    }
    appendLog("State updated by technician");
    sendJson(200, getStateJson());
  });

  server.on("/api/log", HTTP_POST, []() {
    String body = server.arg("plain");
    JsonDocument doc;
    if (deserializeJson(doc, body) != DeserializationError::Ok || !doc["message"].is<const char*>()) {
      sendJson(400, "{\"error\":\"Expected {message:string}\"}");
      return;
    }
    appendLog(doc["message"].as<String>());
    sendJson(200, "{\"ok\":true}");
  });

  server.on("/api/export/log.csv", HTTP_GET, []() {
    JsonDocument doc;
    if (deserializeJson(doc, getStateJson()) != DeserializationError::Ok) {
      sendJson(500, "{\"error\":\"Could not parse state\"}");
      return;
    }

    String csv = "timestamp,message\n";
    for (JsonObject log : doc["logs"].as<JsonArray>()) {
      csv += csvEscape(log["timestamp"].as<String>());
      csv += ",";
      csv += csvEscape(log["message"].as<String>());
      csv += "\n";
    }

    server.sendHeader("Content-Disposition", "attachment; filename=activity-log.csv");
    server.send(200, "text/csv", csv);
  });

  server.on("/api/reset", HTTP_POST, []() {
    String init = defaultStateJson();
    if (!writeFile(STATE_FILE, init)) {
      sendJson(500, "{\"error\":\"Failed to reset state\"}");
      return;
    }
    appendLog("Demo data reset");
    sendJson(200, getStateJson());
  });

  server.on("/generate_204", HTTP_ANY, handleCaptivePortal);
  server.on("/hotspot-detect.html", HTTP_ANY, handleCaptivePortal);
  server.on("/ncsi.txt", HTTP_ANY, handleCaptivePortal);
  server.on("/connecttest.txt", HTTP_ANY, handleCaptivePortal);
  server.on("/redirect", HTTP_ANY, handleCaptivePortal);

  server.onNotFound(handleCaptivePortal);
}
}  // namespace

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    return;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  delay(100);

  dns.start(DNS_PORT, "*", WiFi.softAPIP());

  mountRoutes();
  server.begin();

  appendLog("Portal boot complete");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  dns.processNextRequest();
  server.handleClient();
}

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MG5850BClient.h>

using namespace Victor::Components;

SoftwareSerial _serial;
MG5850BClient _client;

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100); // at least light for some time
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);

  _serial.begin(9600, SWSERIAL_8N1, 4, 5, false, 256);
  _client.onCommand = [](const uint8_t* payload, const size_t size) {
    _serial.write(payload, size);
  };

  pinMode(LED_BUILTIN, OUTPUT);
  ledOn();

  Serial.println("setup complete");
  ledOff();
}

void printEnabled(const bool enabled) {
  auto state = String(enabled ? "enabled" : "disabled");
  Serial.println("[" + String(millis()) + "] func " + state);
}
void printLevel(const uint8_t level) {
  auto state = String(level);
  Serial.println("[" + String(millis()) + "] level " + state);
}
void printValue(const uint16_t value) {
  auto state = String(value);
  Serial.println("[" + String(millis()) + "] value " + state);
}

void loop() {
  while (_serial.available()) {
    auto ch = _serial.read();
    _client.receive((uint8_t)ch);
  }

  auto message = String("");
  while (Serial.available()) {
    message += (char)Serial.read();
    delay(2);
  }

  if (message.length() > 0) {
    ledOn();
    Serial.println("[" + String(millis()) + "] input: " + message);
    if (message.indexOf("setRadarEnable1") == 0) {
      _client.setRadarEnable(true, printEnabled);
    } else if (message.indexOf("setRadarEnable0") == 0) {
      _client.setRadarEnable(false, printEnabled);
    } else if (message.indexOf("getRadarEnable") == 0) {
      _client.getRadarEnable(printEnabled);
    } else if (message.indexOf("setRadarDistance5") == 0) {
      _client.setRadarDistance(5, printLevel);
    } else if (message.indexOf("getRadarDistance") == 0) {
      _client.getRadarDistance(printLevel);
    } else if (message.indexOf("setLightEnable1") == 0) {
      _client.setLightEnable(true, printEnabled);
    } else if (message.indexOf("setLightEnable0") == 0) {
      _client.setLightEnable(false, printEnabled);
    } else if (message.indexOf("getLightEnable") == 0) {
      _client.getLightEnable(printEnabled);
    } else if (message.indexOf("getLightHigh") == 0) {
      _client.getLightHigh(printValue);
    } else if (message.indexOf("setLightHigh800") == 0) {
      _client.setLightHigh(800, printValue);
    } else if (message.indexOf("getLightLow") == 0) {
      _client.getLightLow(printValue);
    } else if (message.indexOf("setLightLow750") == 0) {
      _client.setLightLow(750, printValue);
    }
    ledOff();
  }
}

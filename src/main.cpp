#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MG5850BClient.h>

using namespace Victor::Components;

SoftwareSerial swSerial;
MG5850BClient client;

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
}
void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}
void ledFlash(unsigned long ms = 100) {
  ledOn();
  delay(ms); // at least light for some time
  ledOff();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  swSerial.begin(9600, SWSERIAL_8N1, 4, 5, false, 256);
  client.onCommand = [](const uint8_t* payload, const size_t size) {
    swSerial.write(payload, size);
  };

  Serial.println();
  Serial.println("setup complete");
  ledFlash(500);
}

void printEnabled(const bool enabled) {
  auto state = String(enabled ? "enabled" : "disabled");
  Serial.println("[" + String(millis()) + "] func " + state);
}
void printValue(const uint16_t value) {
  auto state = String(value);
  Serial.println("[" + String(millis()) + "] value " + state);
}

String message = "";
bool isEnterPressed = false;

void loop() {
  while (swSerial.available()) {
    auto ch = swSerial.read();
    client.receive((uint8_t)ch);
  }

  isEnterPressed = false;
  while (Serial.available()) {
    const auto ch = Serial.read();
    if (!isEnterPressed) {
      isEnterPressed = (ch == '\r');
      if (!isEnterPressed) {
        message += (char)ch;
      }
    }
  }

  if (isEnterPressed) {
    message.trim();
    const auto idx = message.indexOf(" ");
    const auto cmd = idx > 0 ? message.substring(0, idx) : message;
    const auto arg = idx > 0 ? message.substring(idx + 1) : "";
    // call apis
    if (cmd == "getRadarEnable") {
      client.getRadarEnable(printEnabled);
    } else if (cmd == "setRadarEnable") {
      const auto enable = (arg == "1");
      client.setRadarEnable(enable, printEnabled);
    } else if (cmd == "getRadarDistance") {
      client.getRadarDistance(printValue);
    } else if (cmd == "setRadarDistance") {
      const auto distance = arg.toInt();
      client.setRadarDistance(distance, printValue);
    } else if (cmd == "getDelayTime") {
      client.getDelayTime(printValue);
    } else if (cmd == "setDelayTime") {
      const auto delay = arg.toInt();
      client.setDelayTime(delay, printValue);
    } else if (cmd == "getLightEnable") {
      client.getLightEnable(printEnabled);
    } else if (cmd == "setLightEnable") {
      const auto enable = (arg == "1");
      client.setLightEnable(enable, printEnabled);
    } else if (cmd == "getLightHigh") {
      client.getLightHigh(printValue);
    } else if (cmd == "setLightHigh") {
      const auto value = arg.toInt();
      client.setLightHigh(value, printValue);
    } else if (cmd == "getLightLow") {
      client.getLightLow(printValue);
    } else if (cmd == "setLightLow") {
      const auto value = arg.toInt();
      client.setLightLow(value, printValue);
    }
    // led
    ledFlash();
    // reset
    message = "";
  }
}

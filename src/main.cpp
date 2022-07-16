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

void printRadar(bool enabled) {
  auto state = String(enabled ? "enabled" : "disabled");
  Serial.println("[" + String(millis()) + "] radar: " + state);
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
    if (message.indexOf("true") == 0) {
      _client.setRadarEnable(true, printRadar);
    } else if (message.indexOf("false") == 0) {
      _client.setRadarEnable(false, printRadar);
    } else if (message.indexOf("read") == 0) {
      _client.getRadarEnable(printRadar);
    }
    ledOff();
  }
}

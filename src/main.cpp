#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mg5850b;

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100); // at least light for some time
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);

  mg5850b.begin(9600, SWSERIAL_8N1, 4, 5, false, 256);

  pinMode(LED_BUILTIN, OUTPUT);
  ledOn();

  Serial.println("setup complete");
  ledOff();
}

uint8_t command[] = {
  0x55,
  0x5A,
  0x81,
  0x00,
  0x00,
  0x81,
  0xFE,
};

String response;

void loop() {
  auto message = String("");
  while (Serial.available()) {
    message += (char)Serial.read();
    delay(2);
  }

  while (mg5850b.available()) {
    auto ch = (byte)mg5850b.read();
    response += String(ch, HEX);
  }

  if (response.endsWith("fe")) {
    Serial.println("got response [" + response + "]");
    response = "";
  }

  auto now = millis();
  if (message.length() > 0) {
    ledOn();
    Serial.println("[" + String(now) + "] got message: " + message);
    if (message.indexOf("1") == 0) {
      mg5850b.write(command, 7);
      Serial.println("sent to mg5850b");
    }
    ledOff();
  }
}

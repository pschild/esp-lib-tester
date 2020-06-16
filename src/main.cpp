#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MqttHandler.h>
#include <OTAUpdateHandler.h>

#ifndef WIFI_SSID
  #error "Missing WIFI_SSID"
#endif

#ifndef WIFI_PASSWORD
  #error "Missing WIFI_PASSWORD"
#endif

#ifndef VERSION
  #error "Missing VERSION"
#endif

MqttHandler mqttHandler("192.168.178.28", String("ESP_") + String(ESP.getChipId()));
OTAUpdateHandler updateHandler("192.168.178.28:9042", VERSION);

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long wifiConnectStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - wifiConnectStart > 5000) {
      return;
    }
  }
}

void onFooBar(char* payload) {
  Serial.print("onFooBar");
  if (strcmp(payload, "on") == 0) {
    digitalWrite(LED_BUILTIN, LOW);
  } else if (strcmp(payload, "off") == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void cb() {
  Serial.println("connected callback");
  mqttHandler.subscribe("/foo/bar", onFooBar);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  connectToWifi();

  mqttHandler.setOnConnectedCallback(cb);
  mqttHandler.setup();

  updateHandler.setInterval(30 * 1000);
}

void loop() {
  mqttHandler.loop();
  updateHandler.loop();
}
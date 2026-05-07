#include <WiFi.h>
#include "esp_wifi.h"
#include "GasSensor.h"
#include "EspNowSender.h"

const int pinMQ6 = 35;

uint8_t receiverAddress[] = {0x30, 0x76, 0xF5, 0xF4, 0x14, 0xD4};

GasSensor sensor(pinMQ6, 50);
EspNowSender sender(receiverAddress);

// ----------------------
// TIEMPO ENVÍO
// ----------------------
unsigned long lastSend = 0;
const int interval = 500;

// ----------------------
// WIFI CONTROL
// ----------------------
const char* ssid = "X6 DANIEL";
const char* password = "xdxdxdxd";

unsigned long wifiTimer = 0;
const unsigned long WIFI_RETRY_TIME = 15000;

bool espNowStarted = false;

// ----------------------
// SETUP
// ----------------------
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  wifiTimer = millis();

  Serial.println("[WiFi] Conectando...");
}

// ----------------------
// LOOP
// ----------------------
void loop() {

  // ----------------------
  // WIFI BACKGROUND (NO BLOQUEANTE)
  // ----------------------
  if (WiFi.status() != WL_CONNECTED) {

    if (millis() - wifiTimer > WIFI_RETRY_TIME) {
      Serial.println("[WiFi] Reintentando...");
      WiFi.begin(ssid, password);
      wifiTimer = millis();
    }
  }

  // ----------------------
  // INICIAR ESP-NOW AUTOMÁTICO
  // ----------------------
  if (WiFi.status() == WL_CONNECTED && !espNowStarted) {

    int canal = WiFi.channel();

    Serial.print("[ESP-NOW] Canal detectado: ");
    Serial.println(canal);

    // 🔥 sincronizar canal con el WiFi
    esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE);

    sender.begin(canal);

    espNowStarted = true;
  }

  // ----------------------
  // ENVÍO DE DATOS
  // ----------------------
  if (espNowStarted && millis() - lastSend >= interval) {

    lastSend = millis();

    int gas = sensor.read();

    Serial.print("Gas: ");
    Serial.println(gas);

    sender.send(gas);
  }
}
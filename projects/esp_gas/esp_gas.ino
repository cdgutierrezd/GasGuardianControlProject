#include <WiFi.h>
#include "esp_wifi.h"
#include "GasSensor.h"
#include "EspNowSender.h"

// Pin analógico para sensor MQ-6
const int pinMQ6 = 35;

// Dirección MAC del receptor ESP-NOW
uint8_t receiverAddress[] = {0x30, 0x76, 0xF5, 0xF4, 0x14, 0xD4};

// Instancia del sensor de gas con 50 muestras
GasSensor sensor(pinMQ6, 50);
// Instancia del transmisor ESP-NOW
EspNowSender sender(receiverAddress);

// Control del intervalo de envío (ms)
unsigned long lastSend = 0;
const int interval = 500;

// Credenciales WiFi
const char* ssid = "X6 DANIEL";
const char* password = "xdxdxdxd";

// Control de reconexión WiFi
unsigned long wifiTimer = 0;
const unsigned long WIFI_RETRY_TIME = 15000;

// Bandera para verificar si ESP-NOW está inicializado
bool espNowStarted = false;

// ----------------------
// SETUP
// ----------------------
void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Configurar WiFi en modo estación
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Iniciar temporizador para reintentos de conexión
  wifiTimer = millis();

  Serial.println("WiFi conectando...");
}

void loop() {

  // Manejar reconexión WiFi sin bloquear el programa
  if (WiFi.status() != WL_CONNECTED) {

    if (millis() - wifiTimer > WIFI_RETRY_TIME) {
      Serial.println("WiFi reintentando conexion...");
      WiFi.begin(ssid, password);
      wifiTimer = millis();
    }
  }

  // Iniciar ESP-NOW cuando WiFi está conectado
  if (WiFi.status() == WL_CONNECTED && !espNowStarted) {

    // Obtener el canal WiFi actual
    int canal = WiFi.channel();

    Serial.print("Canal WiFi detectado: ");
    Serial.println(canal);

    // Sincronizar canal ESP-NOW con el canal WiFi
    esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE);

    // Inicializar transmisor ESP-NOW en el canal sincronizado
    sender.begin(canal);

    espNowStarted = true;
  }

  // Enviar datos del sensor cada cierto intervalo
  if (espNowStarted && millis() - lastSend >= interval) {

    lastSend = millis();

    int gas = sensor.read();

    Serial.print("Gas: ");
    Serial.println(gas);

    sender.send(gas);
  }
}
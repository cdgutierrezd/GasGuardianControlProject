#include "EspNowReceiver.h"
#include <WiFi.h>

// Variable estatica para acceder al gestor de gas en el callback
GasManager* EspNowReceiver::gasManager = nullptr;

// Inicializa el protocolo ESP-NOW
void EspNowReceiver::begin(GasManager* manager) {
  gasManager = manager;

  // Inicializa ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] No se pudo inicializar ESP-NOW");
    return;
  }

  // Registra el callback para recibir datos
  esp_now_register_recv_cb(onReceive);
  Serial.println("[ESP-NOW] Receptor iniciado y listo");
}

// Callback que se ejecuta cuando llegan datos por ESP-NOW
void EspNowReceiver::onReceive(const esp_now_recv_info *info, const uint8_t *data, int len) {
  // Convierte los datos recibidos a la estructura
  SensorData incoming;
  memcpy(&incoming, data, sizeof(incoming));

  // Convierte la direccion MAC a string legible
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  
  // Imprime informacion del dato recibido
  Serial.print("[ESP-NOW] MAC: ");
  Serial.print(macStr);
  Serial.print(" | Gas: ");
  Serial.println(incoming.gasValue);

  // Actualiza el gestor de gas con el nuevo valor
  if (gasManager != nullptr) {
    gasManager->update(incoming.gasValue);
  }
}
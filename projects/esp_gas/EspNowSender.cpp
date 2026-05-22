#include "EspNowSender.h"
#include <WiFi.h>
#include "esp_wifi.h"

// Copiar la dirección MAC en el constructor
EspNowSender::EspNowSender(const uint8_t *addr) {
  memcpy(address, addr, 6);
}

void EspNowSender::begin(int channel) {
  // Configurar WiFi en modo estación
  WiFi.mode(WIFI_STA);

  // Establecer el canal para ESP-NOW
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  Serial.print("Canal WiFi configurado: ");
  Serial.println(WiFi.channel());

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  // Configurar información del peer destino
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, address, 6);
  peerInfo.channel = channel;
  peerInfo.encrypt = false;

  // Agregar el peer a la lista de dispositivos conocidos
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar peer");
  }

  Serial.println("ESP-NOW inicializado correctamente");
}

bool EspNowSender::send(int value) {
  // Preparar estructura de datos con el valor del sensor
  SensorData data;
  data.gasValue = value;

  // Enviar datos a través de ESP-NOW
  esp_err_t result = esp_now_send(address, (uint8_t *)&data, sizeof(data));

  // Verificar resultado del envío
  if (result == ESP_OK) {
    Serial.print("Valor enviado: ");
    Serial.println(value);
    return true;
  } else {
    Serial.print("Error en envio, codigo: ");
    Serial.println(result);
    return false;
  }
}
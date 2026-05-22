#ifndef ESPNOW_SENDER_H
#define ESPNOW_SENDER_H

#include <esp_now.h>

// Estructura de datos a transmitir por ESP-NOW
typedef struct {
  int gasValue;
} SensorData;

class EspNowSender {
  private:
    uint8_t address[6];  // Dirección MAC del peer destino

  public:
    // Constructor que recibe la dirección MAC del receptor
    EspNowSender(const uint8_t *addr);

    // Inicializar ESP-NOW en el canal especificado
    void begin(int channel);
    // Enviar valor del sensor al receptor
    bool send(int value);
};

#endif
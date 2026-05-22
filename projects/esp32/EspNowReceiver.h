#ifndef ESPNOW_RECEIVER_H
#define ESPNOW_RECEIVER_H

#include <esp_now.h>
#include "GasManager.h"

// Estructura de datos recibida desde el sensor por ESP-NOW
typedef struct {
  int gasValue;  // Valor del gas del sensor remoto
} SensorData;

// Receptor ESP-NOW para recibir datos del sensor de gas
class EspNowReceiver {
  public:
    // Inicializa el receptor ESP-NOW
    static void begin(GasManager* manager);

  private:
    // Callback cuando se reciben datos
    static void onReceive(const esp_now_recv_info *info, const uint8_t *data, int len);
    // Referencia al gestor de gas para actualizar datos
    static GasManager* gasManager;
};

#endif
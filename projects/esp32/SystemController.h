#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "GasManager.h"

// Controlador del sistema de valvulas, extractor y alarma
class SystemController {
  private:
    int valveRelayPin;      // Pin del rele de la valvula
    int extractorRelayPin;  // Pin del rele del extractor
    int buttonPin;          // Pin del boton de emergencia
    int alarmRelayPin;      // Pin del rele de la alarma

    bool valveClosed;       // Estado de la valvula

    // Detecta cambios en el nivel de peligro del gas
    bool gasWasDanger;

  public:
    // Constructor que recibe los 4 pines a controlar
    SystemController(int valveRelay, int button, int extractorRelay, int alarmRelay);

    // Inicializa los pines
    void begin();

    // Actualiza el estado del sistema segun el gas y boton
    void update(GasManager &gas, int threshold);

    // Retorna si la valvula esta cerrada
    bool isValveClosed();

    // Abre o cierra la valvula desde la app
    void setValve(bool closed);
};

#endif
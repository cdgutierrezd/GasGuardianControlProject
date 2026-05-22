#include "SystemController.h"
#include <Arduino.h>

// Constructor que inicializa los pines
SystemController::SystemController(int valveRelay, int button, int extractorRelay, int alarmRelay) {
  valveRelayPin = valveRelay;
  buttonPin = button;
  extractorRelayPin = extractorRelay;
  alarmRelayPin = alarmRelay;

  valveClosed = false;
  gasWasDanger = false;
}

// Inicializa los pines como entradas o salidas
void SystemController::begin() {
  pinMode(valveRelayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(extractorRelayPin, OUTPUT);
  pinMode(alarmRelayPin, OUTPUT);

  // Abre la valvula por defecto al iniciar
  digitalWrite(valveRelayPin, LOW);
  // Extractor apagado
  digitalWrite(extractorRelayPin, HIGH);
  // Alarma apagada
  digitalWrite(alarmRelayPin, HIGH);

  valveClosed = false;
}

// Actualiza el estado del sistema cada ciclo
void SystemController::update(GasManager &gas, int threshold) {

  bool gasDanger = gas.isDanger(threshold);
  int buttonState = digitalRead(buttonPin);

  // Recuerda si el cierre actual fue provocado por el boton
  static bool cerradoPorBoton = false;

  // Detecta cambio a peligro de gas
  if (gasDanger && !gasWasDanger) {
    // Cierra la valvula
    digitalWrite(valveRelayPin, HIGH);
    // Enciende el extractor
    digitalWrite(extractorRelayPin, LOW);
    // Enciende la alarma
    digitalWrite(alarmRelayPin, LOW);

    valveClosed = true;

    Serial.print("[AUTO] Gas peligroso activado: ");
    Serial.println(gas.getValue());
  }

  // Detecta cuando el gas vuelve a normal
  if (!gasDanger && gasWasDanger) {
    // Apaga el extractor
    digitalWrite(extractorRelayPin, HIGH);
    // Apaga la alarma
    digitalWrite(alarmRelayPin, HIGH);

    // La valvula permanece cerrada por seguridad
    // Se reabre solo desde la app o soltando el boton
    Serial.println("[AUTO] Gas normal: Extractor y alarma apagados");
  }

  // Actualiza el estado previo del gas
  gasWasDanger = gasDanger;

  // Procesa el boton de emergencia
  if (buttonState == HIGH) {
    // Boton presionado: cierra la valvula
    if (!cerradoPorBoton) {
      // Cierra la valvula
      digitalWrite(valveRelayPin, HIGH);
      valveClosed = true;
      cerradoPorBoton = true;
      Serial.println("[BOTON] Emergencia activada - Valvula cerrada");
    }
  } 
  else {
    // Boton soltado: intenta reabrir si no hay peligro
    if (cerradoPorBoton) {
      // Solo abre si no hay peligro de gas
      if (!gasDanger) {
        // Abre la valvula
        digitalWrite(valveRelayPin, LOW);
        valveClosed = false;
        Serial.println("[BOTON] Emergencia liberada - Valvula reabierta");
      } else {
        Serial.println("[BOTON] Intento de reapertura cancelado: Gas detectado");
      }
      
      // Limpia la bandera al soltar el boton
      cerradoPorBoton = false; 
    }
  }
}

// Retorna el estado actual de la valvula
bool SystemController::isValveClosed() {
  return valveClosed;
}

// Controla la valvula desde la aplicacion
void SystemController::setValve(bool closed) {
  valveClosed = closed;
  digitalWrite(valveRelayPin, closed ? HIGH : LOW);
  Serial.println(closed ? "[APP] Valvula cerrada" : "[APP] Valvula abierta");
}
#include "SystemController.h"
#include <Arduino.h>

// ----------------------
// CONSTRUCTOR
// ----------------------
SystemController::SystemController(int valveRelay, int button, int extractorRelay, int alarmRelay) {
  valveRelayPin = valveRelay;
  buttonPin = button;
  extractorRelayPin = extractorRelay;
  alarmRelayPin = alarmRelay;

  valveClosed = false;
  gasWasDanger = false;
}

// ----------------------
// INIT
// ----------------------
void SystemController::begin() {
  pinMode(valveRelayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(extractorRelayPin, OUTPUT);
  pinMode(alarmRelayPin, OUTPUT);

  digitalWrite(valveRelayPin, LOW);      // 🟢 Válvula ABIERTA por defecto al iniciar (Relé ON)
  digitalWrite(extractorRelayPin, HIGH);  // Extractor apagado
  digitalWrite(alarmRelayPin, HIGH);       // Alarma apagada

  valveClosed = false;
}

// ----------------------
// UPDATE
// ----------------------
void SystemController::update(GasManager &gas, int threshold) {

  bool gasDanger = gas.isDanger(threshold);
  int buttonState = digitalRead(buttonPin);

  // 🧠 MEMORIA INTERNA: Recuerda si el cierre actual fue provocado por el botón
  static bool cerradoPorBoton = false;

  // ----------------------
  // 🔴 GAS (Evento sincronizado de peligro)
  // ----------------------
  if (gasDanger && !gasWasDanger) {
    digitalWrite(valveRelayPin, HIGH);       // Cerrar válvula (Relé OFF)
    digitalWrite(extractorRelayPin, LOW);    // Prender extractor
    digitalWrite(alarmRelayPin, LOW);        // Prender alarma

    valveClosed = true;

    Serial.print("[AUTO] Gas peligroso → TODO ACTIVADO: ");
    Serial.println(gas.getValue());
  }

  // ----------------------
  // 🔄 GAS vuelve a normal
  // ----------------------
  if (!gasDanger && gasWasDanger) {
    digitalWrite(extractorRelayPin, HIGH);   // Apagar extractor
    digitalWrite(alarmRelayPin, HIGH);       // Apagar alarma

    // NOTA SEGURIDAD: La válvula permanece CERRADA. 
    // Solo se reabrirá mediante orden explícita de la App o ciclando el botón manual.
    Serial.println("[AUTO] Gas normal → Extractor y alarma apagados. Válvula segura (Cerrada).");
  }

  // 🔥 IMPORTANTE: Actualizar estado de referencia del gas
  gasWasDanger = gasDanger;

  // ----------------------
  // 🔘 BOTÓN (Cierre por emergencia y Apertura al soltar)
  // ----------------------
  if (buttonState == HIGH) {
    // Si el botón está activo y aún no habíamos registrado este cierre manual...
    if (!cerradoPorBoton) {
      digitalWrite(valveRelayPin, HIGH); // Apagar relé -> Válvula CERRADA
      valveClosed = true;
      cerradoPorBoton = true;            // El botón toma la autoría del bloqueo
      Serial.println("[BOTÓN] Emergencia manual activa → Válvula CERRADA");
    }
  } 
  else {
    // Si el botón vuelve a su estado de reposo (LOW) y él fue quien provocó el cierre...
    if (cerradoPorBoton) {
      // Condición crítica: Solo rearmamos si el sensor confirma que el aire está limpio
      if (!gasDanger) {
        digitalWrite(valveRelayPin, LOW); // Encender relé -> Válvula ABIERTA
        valveClosed = false;
        Serial.println("[BOTÓN] Emergencia liberada → Válvula REABIERTA");
      } else {
        Serial.println("[BOTÓN] Intento de rearme cancelado: Aún se detecta peligro de GAS");
      }
      
      // Limpiamos la bandera en cualquier caso al soltar el botón
      cerradoPorBoton = false; 
    }
  }
}

// ----------------------
// ESTADO
// ----------------------
bool SystemController::isValveClosed() {
  return valveClosed;
}

// ----------------------
// APP
// ----------------------
void SystemController::setValve(bool closed) {
  valveClosed = closed;
  digitalWrite(valveRelayPin, closed ? HIGH : LOW);
  Serial.println(closed ? "[APP] Válvula CERRADA" : "[APP] Válvula ABIERTA");
}
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

  digitalWrite(valveRelayPin, LOW);      // 🔴 válvula CERRADA
  digitalWrite(extractorRelayPin, HIGH);  // extractor apagado
  digitalWrite(alarmRelayPin, HIGH);      // alarma apagada

  valveClosed = false;
}

// ----------------------
// UPDATE
// ----------------------
void SystemController::update(GasManager &gas, int threshold) {

  bool gasDanger = gas.isDanger(threshold);
  int buttonState = digitalRead(buttonPin);

  // ----------------------
  // 🔴 GAS (evento sincronizado)
  // ----------------------
  if (gasDanger && !gasWasDanger) {

    digitalWrite(valveRelayPin, HIGH);       // cerrar válvula
    digitalWrite(extractorRelayPin, LOW);    // prender extractor
    digitalWrite(alarmRelayPin, LOW);        // prender alarma

    valveClosed = true;

    Serial.print("[AUTO] Gas peligroso → TODO ACTIVADO: ");
    Serial.println(gas.getValue());
  }

  // ----------------------
  // 🔄 GAS vuelve a normal
  // ----------------------
  if (!gasDanger && gasWasDanger) {

    digitalWrite(extractorRelayPin, HIGH);   // apagar extractor
    digitalWrite(alarmRelayPin, HIGH);       // apagar alarma

    Serial.println("[AUTO] Gas normal → extractor apagado");
  }

  // 🔥 IMPORTANTE: actualizar estado
  gasWasDanger = gasDanger;

  // ----------------------
  // 🔘 BOTÓN (solo cerrar)
  // ----------------------
  if (buttonState == HIGH) {

    digitalWrite(valveRelayPin, HIGH);
    valveClosed = true;

    Serial.println("[BOTÓN] Válvula CERRADA");

    delay(300); // anti rebote simple
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
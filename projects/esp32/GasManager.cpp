#include "GasManager.h"
#include <Arduino.h>

// ----------------------
// CONSTRUCTOR
// ----------------------
GasManager::GasManager() {
  value = 0;
  rawValue = 0;
  index = 0;
  gasState = false;
  gasHighStart = 0;

  for (int i = 0; i < FILTER_SIZE; i++) {
    readings[i] = 0;
  }
}

// ----------------------
// UPDATE (recibe ESP-NOW)
// ----------------------
void GasManager::update(int newValue) {

  rawValue = newValue;

  // 🔥 1. FILTRO PROMEDIO
  readings[index] = rawValue;
  index = (index + 1) % FILTER_SIZE;

  int sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += readings[i];
  }

  value = sum / FILTER_SIZE;
}

// ----------------------
// GET VALOR FILTRADO
// ----------------------
int GasManager::getValue() {
  return value;
}

// ----------------------
// DETECCIÓN INTELIGENTE
// ----------------------
bool GasManager::isDanger(int threshold) {

  const int THRESHOLD_ON = threshold;
  const int THRESHOLD_OFF = threshold - 100;

  // 🔥 HISTÉRESIS + TIEMPO
  if (!gasState && value > THRESHOLD_ON) {

    if (gasHighStart == 0) {
      gasHighStart = millis();
    }

    if (millis() - gasHighStart > CONFIRM_TIME) {
      gasState = true;
    }

  } 
  else if (gasState && value < THRESHOLD_OFF) {

    gasState = false;
    gasHighStart = 0;
  }

  // reset si baja antes de confirmar
  if (value <= THRESHOLD_ON) {
    gasHighStart = 0;
  }

  return gasState;
}
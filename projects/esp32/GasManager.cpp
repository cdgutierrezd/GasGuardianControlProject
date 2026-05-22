#include "GasManager.h"
#include <Arduino.h>

// Constructor inicializa las variables
GasManager::GasManager() {
  value = 0;
  rawValue = 0;
  index = 0;
  gasState = false;
  gasHighStart = 0;

  // Inicializa el buffer del filtro
  for (int i = 0; i < FILTER_SIZE; i++) {
    readings[i] = 0;
  }
}

// Actualiza el valor del gas recibido del sensor
void GasManager::update(int newValue) {

  rawValue = newValue;

  // Aplica filtro de promedio movil
  readings[index] = rawValue;
  index = (index + 1) % FILTER_SIZE;

  // Calcula el promedio de las ultimas 10 lecturas
  int sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += readings[i];
  }

  value = sum / FILTER_SIZE;
}

// Retorna el valor filtrado del gas
int GasManager::getValue() {
  return value;
}

// Detecta peligro de gas con histéresis y confirmacion por tiempo
bool GasManager::isDanger(int threshold) {

  const int THRESHOLD_ON = threshold;
  // Histéresis: baja el umbral para desactivar
  const int THRESHOLD_OFF = threshold - 100;

  // Si el gas sube por encima del umbral
  if (!gasState && value > THRESHOLD_ON) {

    // Comienza a contar el tiempo
    if (gasHighStart == 0) {
      gasHighStart = millis();
    }

    // Cambia a peligro despues de 2 segundos confirmados
    if (millis() - gasHighStart > CONFIRM_TIME) {
      gasState = true;
    }

  } 
  // Si el gas baja por debajo del umbral bajo
  else if (gasState && value < THRESHOLD_OFF) {
    // Vuelve a normal
    gasState = false;
    gasHighStart = 0;
  }

  // Resetea el contador si el gas baja antes de confirmar
  if (value <= THRESHOLD_ON) {
    gasHighStart = 0;
  }

  return gasState;
}
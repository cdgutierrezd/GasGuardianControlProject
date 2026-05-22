#include "GasSensor.h"
#include <Arduino.h>

// Inicializar el sensor con el pin y número de muestras
GasSensor::GasSensor(int p, int s) {
  pin = p;
  samples = s;
}

int GasSensor::read() {
  // Acumular lecturas del sensor
  long sum = 0;

  // Tomar múltiples muestras con pequeña pausa entre ellas
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(200);  // Pequeño retardo para estabilidad
  }

  // Retornar el valor promediado
  return sum / samples;
}
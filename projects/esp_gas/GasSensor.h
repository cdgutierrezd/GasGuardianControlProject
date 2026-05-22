#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

class GasSensor {
  private:
    int pin;      // Pin analógico del sensor MQ-6
    int samples;  // Número de muestras para promediar

  public:
    // Constructor que recibe el pin y número de muestras (por defecto 50)
    GasSensor(int pin, int samples = 50);

    // Leer valor promediado del sensor
    int read();
};

#endif
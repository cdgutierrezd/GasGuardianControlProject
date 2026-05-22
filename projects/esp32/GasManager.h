#ifndef GAS_MANAGER_H
#define GAS_MANAGER_H

// Gestiona las lecturas del sensor de gas
class GasManager {
  private:
    int value;           // Valor filtrado del gas
    int rawValue;        // Ultimo valor recibido del sensor

    // Filtro de promedio movil para estabilizar lecturas
    static const int FILTER_SIZE = 10;
    int readings[FILTER_SIZE];
    int index;

    // Estado actual del gas
    bool gasState;

    // Tiempo de confirmacion para detectar peligro real
    unsigned long gasHighStart;
    const unsigned long CONFIRM_TIME = 2000; // 2 segundos

  public:
    // Constructor
    GasManager();

    // Actualiza el valor del gas recibido del sensor
    void update(int newValue);

    // Retorna el valor filtrado del gas
    int getValue();

    // Verifica si hay peligro de gas con histéresis
    bool isDanger(int threshold);
};

#endif
#ifndef GAS_MANAGER_H
#define GAS_MANAGER_H

class GasManager {
  private:
    int value;           // valor filtrado
    int rawValue;        // último valor recibido

    // 🔥 FILTRO PROMEDIO
    static const int FILTER_SIZE = 10;
    int readings[FILTER_SIZE];
    int index;

    // 🔥 HISTÉRESIS
    bool gasState;

    // 🔥 TIEMPO DE CONFIRMACIÓN
    unsigned long gasHighStart;
    const unsigned long CONFIRM_TIME = 2000; // 2 segundos

  public:
    GasManager();

    void update(int newValue);
    int getValue();
    bool isDanger(int threshold);
};

#endif
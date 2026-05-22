#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <lvgl.h>
#include "ui.h"
#include "ui_Screen1.h"

class GasManager;
class SystemController;

// Gestor de la interfaz de usuario
class DisplayManager {
  public:
    // Inicializa la UI
    void begin();

    // Actualiza los valores en pantalla
    void update(int gasValue, bool valveClosed, bool gasDanger);

    // Actualiza la informacion de WiFi
    void updateWifiInfo(bool connected, String ssid, String ip);

    // Muestra la pantalla de carga
    void showLoadingScreen();

    // Muestra la pantalla principal
    void showMainScreen();

    // Cambia entre pantallas
    void setScreen(int screen);

  private:
    int screenState = 0;  // 0 = principal, 1 = carga
};

#endif
#include <Arduino.h>
#include "DisplayManager.h"

// Inicializa la interfaz de usuario
void DisplayManager::begin() {
  // Carga todos los elementos de la UI
  ui_init();

  // Configura el rango del arco de gas de 0 a 100 porciento
  lv_arc_set_range(ui_uiArcGas, 0, 100);

  // Muestra la pantalla de carga al iniciar
  showLoadingScreen();
}

// Cambia entre pantalla principal y pantalla de carga
void DisplayManager::setScreen(int screen) {
  screenState = screen;

  // Pantalla principal
  if (screenState == 0) {
    showMainScreen();
  } 
  // Pantalla de carga
  else {
    showLoadingScreen();
  }
}

// Carga la pantalla principal
void DisplayManager::showMainScreen() {
  lv_scr_load(ui_Screen1);
}

// Carga la pantalla de carga
void DisplayManager::showLoadingScreen() {
  lv_scr_load(ui_Screen2);
}

// Actualiza los valores mostrados en la pantalla
void DisplayManager::update(int gasValue, bool valveClosed, bool gasDanger) {

  // Limita las actualizaciones a 50ms para no sobrecargar la pantalla
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 50) return;
  lastUpdate = millis();

  // Solo actualiza si estamos en la pantalla principal
  if (screenState != 0) return;

  // Actualiza el label con el valor del gas
  lv_label_set_text_fmt(ui_uiLabelGasValue, "%d", gasValue);

  // Convierte el valor de gas a porcentaje para el arco
  int BASE = 0;           // Valor minimo
  int PELIGRO = 1500;     // Valor maximo

  // Calcula el porcentaje
  int porcentaje = (gasValue - BASE) * 100 / (PELIGRO - BASE);

  // Limita el porcentaje entre 0 y 100
  if (porcentaje < 0) porcentaje = 0;
  if (porcentaje > 100) porcentaje = 100;

  // Suaviza el cambio de porcentaje para que no sea brusco
  static int porcentajeSuave = 0;
  porcentajeSuave = porcentajeSuave * 0.8 + porcentaje * 0.2;

  // Actualiza el arco del gas
  lv_arc_set_value(ui_uiArcGas, porcentajeSuave);

  // Actualiza el texto de estado
  if (gasDanger) {
    lv_label_set_text(ui_uiLabelState, "ALERTA");
  } else {
    lv_label_set_text(ui_uiLabelState, "NORMAL");
  }

  // Actualiza el boton de valvula segun su estado
  if (!valveClosed) {
    lv_obj_add_state(ui_uiBtnValve, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(ui_uiBtnValve, LV_STATE_CHECKED);
  }

  // Actualiza el boton del extractor segun el estado del gas
  if (gasDanger) {
    lv_obj_add_state(ui_uiBtnExtractor, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(ui_uiBtnExtractor, LV_STATE_CHECKED);
  }
}

// Actualiza la informacion de WiFi en pantalla
void DisplayManager::updateWifiInfo(bool connected, String ssid, String ip) {

  // Solo actualiza si estamos en la pantalla principal
  if (screenState != 0) return;

  // Si hay conexion WiFi
  if (connected) {
    // Muestra el nombre de la red
    String wifiText = "WIFI: " + ssid;
    // Muestra la direccion IP
    String ipText = "IP: " + ip;

    lv_label_set_text(ui_uiLabelWifi, wifiText.c_str());
    lv_label_set_text(ui_uiLabelIp, ipText.c_str());

  } 
  // Si no hay conexion
  else {
    lv_label_set_text(ui_uiLabelWifi, "WIFI: NOT CONNECTED");
    lv_label_set_text(ui_uiLabelIp, "IP: ---");
  }
}
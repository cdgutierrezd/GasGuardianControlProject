#include <Arduino.h>
#include "DisplayManager.h"


// ----------------------
// INIT UI
// ----------------------
void DisplayManager::begin() {
  ui_init();

  lv_arc_set_range(ui_uiArcGas, 0, 100);

  // 🔥 ARRANCA EN PANTALLA DE CARGA
  showLoadingScreen();
}

// ----------------------
// CAMBIAR PANTALLA (CONTROL CENTRAL)
// ----------------------
void DisplayManager::setScreen(int screen) {
  screenState = screen;

  if (screenState == 0) {
    showMainScreen();
  } else {
    showLoadingScreen();
  }
}

// ----------------------
// SCREEN PRINCIPAL (UI NORMAL)
// ----------------------
void DisplayManager::showMainScreen() {
  lv_scr_load(ui_Screen1);
}

// ----------------------
// SCREEN DE CARGA (Screen2)
// ----------------------
void DisplayManager::showLoadingScreen() {
  lv_scr_load(ui_Screen2);

}

// ----------------------
// UPDATE UI (TU LÓGICA ACTUAL)
// ----------------------
void DisplayManager::update(int gasValue, bool valveClosed, bool gasDanger) {


  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < 50) return;
  lastUpdate = millis();

  if (screenState != 0) return;

  // LABEL GAS
  lv_label_set_text_fmt(ui_uiLabelGasValue, "%d", gasValue);

  // ARC
  // 🔥 ESCALADO REAL
  int BASE = 0;
  int PELIGRO = 1500;

  int porcentaje = (gasValue - BASE) * 100 / (PELIGRO - BASE);

  if (porcentaje < 0) porcentaje = 0;
  if (porcentaje > 100) porcentaje = 100;

  // 🔥 SUAVIZADO
  static int porcentajeSuave = 0;
  porcentajeSuave = porcentajeSuave * 0.8 + porcentaje * 0.2;

  lv_arc_set_value(ui_uiArcGas, porcentajeSuave);

  // 🔥 ESTADO (USA gasDanger)
  if (gasDanger) {
    lv_label_set_text(ui_uiLabelState, "ALERTA");
  } else {
    lv_label_set_text(ui_uiLabelState, "NORMAL");
  }

  // 🔥 BOTÓN VÁLVULA
  if (!valveClosed) {
    lv_obj_add_state(ui_uiBtnValve, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(ui_uiBtnValve, LV_STATE_CHECKED);
  }

  // 🔥 BOTÓN EXTRACTOR (SINCRONIZADO)
  if (gasDanger) {
    lv_obj_add_state(ui_uiBtnExtractor, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(ui_uiBtnExtractor, LV_STATE_CHECKED);
  }
}
void DisplayManager::updateWifiInfo(bool connected, String ssid, String ip) {

  if (screenState != 0) return; // solo Screen1

  if (connected) {

    String wifiText = "WIFI: " + ssid;
    String ipText = "IP: " + ip;

    lv_label_set_text(ui_uiLabelWifi, wifiText.c_str());
    lv_label_set_text(ui_uiLabelIp, ipText.c_str());

  } else {

    lv_label_set_text(ui_uiLabelWifi, "WIFI: NOT CONNECTED");
    lv_label_set_text(ui_uiLabelIp, "IP: ---");
  }
}
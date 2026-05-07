#include <WiFi.h>
#include "GasManager.h"
#include "EspNowReceiver.h"
#include "SystemController.h"
#include "ApiServer.h"
#include "DisplayManager.h"
#include "DisplayDriver.h"


// ----------------------
// CONFIG GAS
// ----------------------
#define GAS_PELIGRO 2000
// ----------------------
// WIFI CONFIG
// ----------------------
const char* ssid = "X6 DANIEL";
const char* password = "xdxdxdxd";

// ----------------------
// TIEMPOS
// ----------------------
const unsigned long SCREEN_LOADING_TIME = 3000;
const unsigned long WIFI_RETRY_TIME = 15000;

// ----------------------
// OBJETOS
// ----------------------
GasManager gas;
SystemController controller(5, 4, 2, 13);
ApiServer api;
DisplayDriver driver;
DisplayManager display;

// ----------------------
// ESTADOS
// ----------------------
enum SystemState {
  STARTING,
  RUNNING
};

SystemState state = STARTING;

// ----------------------
// TIMERS
// ----------------------
unsigned long wifiTimer = 0;
unsigned long screenStartTime = 0;

// ----------------------
// CONTROL ESP-NOW
// ----------------------
bool espNowStarted = false;

// ----------------------
// SETUP
// ----------------------
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== SISTEMA INICIANDO ===");

  // DISPLAY
  driver.begin();
  display.begin();
  display.setScreen(1); // loading

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  wifiTimer = millis();
  screenStartTime = millis();

  Serial.println("[WiFi] Intentando conexión...");

  // SISTEMA BASE
  controller.begin();

  Serial.println("[API] Iniciando servidor...");
  api.begin(&gas, &controller, 600);

  Serial.println("=== SISTEMA LISTO ===\n");
}

// ----------------------
// LOOP
// ----------------------
void loop() {

  // 🔥 UI SIEMPRE ACTIVA
  driver.loop();

  // ----------------------
  // WIFI BACKGROUND
  // ----------------------
  if (WiFi.status() != WL_CONNECTED) {

    if (millis() - wifiTimer > WIFI_RETRY_TIME) {

      Serial.println("[WiFi] Reintentando...");

      WiFi.begin(ssid, password,8); // 🔥 SIN disconnect()

      wifiTimer = millis();
    }
  }

  // ----------------------
  // TRANSICIÓN UI
  // ----------------------
  if (state == STARTING) {

    display.setScreen(1);

    if (millis() - screenStartTime > SCREEN_LOADING_TIME) {

      Serial.println("[UI] pasando a pantalla principal");

      display.setScreen(0);
      state = RUNNING;
    }

    return;
  }

  // ----------------------
  // RUNNING
  // ----------------------
  if (state == RUNNING) {

    // 🔥 INICIAR ESP-NOW CUANDO WIFI YA ESTÉ CONECTADO
    if (WiFi.status() == WL_CONNECTED && !espNowStarted) {

      Serial.println("[ESP-NOW] Iniciando...");

      EspNowReceiver::begin(&gas);

      espNowStarted = true;
    }

  // SISTEMA

  controller.update(gas, GAS_PELIGRO);

  int gasValue = gas.getValue();
  bool valveClosed = controller.isValveClosed();

  bool gasDanger = gas.isDanger(GAS_PELIGRO);
  display.update(gasValue, valveClosed, gasDanger);
    // WIFI UI
    display.updateWifiInfo(
      WiFi.status() == WL_CONNECTED,
      WiFi.SSID(),
      WiFi.localIP().toString()
    );

    api.handle();
  }
}
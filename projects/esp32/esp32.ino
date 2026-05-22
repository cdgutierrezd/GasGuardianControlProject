#include <WiFi.h>
#include "GasManager.h"
#include "EspNowReceiver.h"
#include "SystemController.h"
#include "ApiServer.h"
#include "DisplayManager.h"
#include "DisplayDriver.h"

// Umbral de peligro para el gas
#define GAS_PELIGRO 2000

// Configuracion de WiFi
const char* ssid = "X6 DANIEL";
const char* password = "xdxdxdxd";

// Tiempos en milisegundos
const unsigned long SCREEN_LOADING_TIME = 3000;  // Tiempo que dura la pantalla de carga
const unsigned long WIFI_RETRY_TIME = 15000;      // Tiempo entre intentos de WiFi

// Instancias de los managers
GasManager gas;                                    // Gestor del sensor de gas
SystemController controller(5, 4, 2, 13);         // Controlador: valvula, boton, extractor, alarma
ApiServer api;                                     // Servidor web
DisplayDriver driver;                              // Driver bajo nivel de pantalla
DisplayManager display;                            // Gestor de UI

// Estados del sistema
enum SystemState {
  STARTING,  // Iniciando y mostrando pantalla de carga
  RUNNING    // Sistema en funcionamiento normal
};

SystemState state = STARTING;

// Variables de tiempo
unsigned long wifiTimer = 0;      // Tiempo del ultimo intento de WiFi
unsigned long screenStartTime = 0; // Tiempo de inicio del sistema

// Control de ESP-NOW
bool espNowStarted = false;  // Indica si ESP-NOW ya esta inicializado

// Configuracion inicial del sistema
void setup() {

  // Inicia comunicacion serial
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== SISTEMA INICIANDO ===");

  // Inicializa la pantalla
  driver.begin();
  display.begin();
  display.setScreen(1); // Muestra pantalla de carga

  // Configura WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  wifiTimer = millis();
  screenStartTime = millis();

  Serial.println("[WiFi] Intentando conexion...");

  // Inicializa el controlador del sistema
  controller.begin();

  // Inicia el servidor API
  Serial.println("[API] Iniciando servidor...");
  api.begin(&gas, &controller, GAS_PELIGRO);

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
  // Intenta reconectar si no hay WiFi
  if (WiFi.status() != WL_CONNECTED) {

    if (millis() - wifiTimer > WIFI_RETRY_TIME) {

      Serial.println("[WiFi] Reintentando...");

      // Intenta conectar nuevamente
      WiFi.begin(ssid, password, 8);

      wifiTimer = millis();
    }
  }

  // Estado de inicio: muestra pantalla de carga
  if (state == STARTING) {

    display.setScreen(1);

    // Cambia a pantalla principal despues del tiempo de carga
    if (millis() - screenStartTime > SCREEN_LOADING_TIME) {

      Serial.println("[UI] Cambiando a pantalla principal");

      display.setScreen(0);
      state = RUNNING;
    }

    return;
  }

  // Estado de funcionamiento normal
  if (state == RUNNING) {

    // Inicializa ESP-NOW cuando WiFi se conecta
    if (WiFi.status() == WL_CONNECTED && !espNowStarted) {

      Serial.println("[ESP-NOW] Iniciando receptor...");

      EspNowReceiver::begin(&gas);

      espNowStarted = true;
    }

    // Actualiza el controlador del sistema
    controller.update(gas, GAS_PELIGRO);

    // Obtiene los valores actuales
    int gasValue = gas.getValue();
    bool valveClosed = controller.isValveClosed();
    bool gasDanger = gas.isDanger(GAS_PELIGRO);

    // Actualiza la pantalla con los valores
    display.update(gasValue, valveClosed, gasDanger);

    // Actualiza informacion de WiFi
    display.updateWifiInfo(
      WiFi.status() == WL_CONNECTED,
      WiFi.SSID(),
      WiFi.localIP().toString()
    );

    // Procesa solicitudes HTTP
    api.handle();
  }
}
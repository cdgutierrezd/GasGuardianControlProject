#include "ApiServer.h"

// Constructor inicializa el servidor en puerto 80
ApiServer::ApiServer() : server(80) {}

// Inicializa el servidor con referencias a los managers
void ApiServer::begin(GasManager* g, SystemController* c, int t) {
  gas = g;
  controller = c;
  threshold = t;

  // Define las rutas del servidor
  server.on("/status", [this]() { handleStatus(); });
  server.on("/valve", [this]() { handleValve(); });

  // Inicia el servidor
  server.begin();
}

// Procesa solicitudes HTTP pendientes
void ApiServer::handle() {
  server.handleClient();
}

// Retorna el estado actual del sistema en JSON
void ApiServer::handleStatus() {
  // Determina el estado de alerta
  String state = gas->isDanger(threshold) ? "ALERT" : "NORMAL";

  // Construye la respuesta JSON
  String json = "{";
  json += "\"gas\":" + String(gas->getValue()) + ",";
  json += "\"state\":\"" + state + "\",";
  json += "\"valve\":\"" + String(controller->isValveClosed() ? "CLOSED" : "OPEN") + "\"";
  json += "}";

  // Envia la respuesta
  server.send(200, "application/json", json);
}

// Abre o cierra la valvula desde la aplicacion
void ApiServer::handleValve() {
  String action = server.arg("action");

  // Verifica que no haya peligro de gas antes de abrir
  if (action == "open" && gas->isDanger(threshold)) {
    Serial.println("[APP] No se puede abrir: Gas detectado");
    server.send(403, "text/plain", "Gas detected");
    return;
  }

  // Cierra la valvula
  if (action == "close") {
    controller->setValve(true);
    Serial.println("[APP] Valvula cerrada desde aplicacion");
  }
  // Abre la valvula
  if (action == "open") {
    controller->setValve(false);
    Serial.println("[APP] Valvula abierta desde aplicacion");
  }

  // Confirma la operacion
  server.send(200, "text/plain", "OK");
}
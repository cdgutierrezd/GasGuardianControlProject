#ifndef API_SERVER_H
#define API_SERVER_H

#include <WebServer.h>
#include "GasManager.h"
#include "SystemController.h"

// Servidor web para controlar el sistema desde una aplicacion
class ApiServer {
  private:
    WebServer server;           // Servidor web en puerto 80
    GasManager* gas;            // Referencia al gestor de gas
    SystemController* controller; // Referencia al controlador
    int threshold;              // Umbral de peligro

  public:
    // Constructor
    ApiServer();

    // Inicializa el servidor con referencias a los managers
    void begin(GasManager* g, SystemController* c, int t);

    // Procesa las solicitudes HTTP
    void handle();

  private:
    // Maneja solicitud GET /status - retorna estado del sistema
    void handleStatus();

    // Maneja solicitud GET /valve - abre o cierra la valvula
    void handleValve();
};

#endif
#include <Arduino.h>
#include "DisplayDriver.h"

// Variables estaticas para los buffers de LVGL
lv_disp_draw_buf_t DisplayDriver::draw_buf;
lv_color_t DisplayDriver::buf[240 * 10];

// Callback que envia los pixels a la pantalla
void DisplayDriver::my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {

  // Calcula el ancho y alto del area a actualizar
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // Crea una instancia estatica de la pantalla
  static TFT_eSPI tft = TFT_eSPI();

  // Envia los datos a la pantalla
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  // Notifica a LVGL que la escritura termino
  lv_disp_flush_ready(disp);
}

// Inicializa la pantalla y LVGL
void DisplayDriver::begin() {

  // Inicializa LVGL
  lv_init();

  // Inicializa la pantalla TFT
  tft.begin();
  tft.setRotation(0);

  // Configura el buffer de LVGL
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, 240 * 10);

  // Configura el driver de pantalla de LVGL
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  // Resolucion de la pantalla
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 320;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;

  // Registra el driver
  lv_disp_drv_register(&disp_drv);
}

// Actualiza LVGL cada ciclo
void DisplayDriver::loop() {
  // Procesa los eventos pendientes de LVGL
  lv_timer_handler();
  // Incrementa el tick interno
  lv_tick_inc(5);
  // Espera 5ms
  delay(5);
}
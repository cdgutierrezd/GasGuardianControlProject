#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <lvgl.h>
#include <TFT_eSPI.h>

// Driver bajo nivel para la pantalla TFT
class DisplayDriver {
  private:
    TFT_eSPI tft;          // Instancia de la pantalla

    // Buffer para LVGL
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[240 * 10];

    // Callback para enviar datos a la pantalla
    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

  public:
    // Inicializa la pantalla y LVGL
    void begin();

    // Actualiza la pantalla cada ciclo
    void loop();
};

#endif
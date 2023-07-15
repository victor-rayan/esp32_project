#include "driver/touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TOUCH_THRESHOLD     700
#define TOUCH_FILTER_VALUE  10
#define TOUCH_DELAY_MS      200

void touchTask(void *pvParameters) {
    touch_pad_intr_enable();
    while (1) {
        uint32_t pad_status = touch_pad_get_status();
        if (pad_status & (1 << TOUCH_PAD_NUM0)) {
            printf("Pad 1\n");
            touch_pad_clear_status();

            // Desabilita temporariamente a detecção de toque
            touch_pad_config(TOUCH_PAD_NUM0, 0);
            vTaskDelay(pdMS_TO_TICKS(TOUCH_DELAY_MS)); // Pequeno atraso para evitar detecção múltipla
            touch_pad_config(TOUCH_PAD_NUM0, TOUCH_THRESHOLD);
        }
        // Lidar com outros pads, se necessário
        vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno atraso antes de verificar novamente
    }
}

void initTouch() {
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_NUM0, TOUCH_THRESHOLD);
    touch_pad_filter_start(TOUCH_FILTER_VALUE);
}

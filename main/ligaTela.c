#include "driver/touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>

#define TOUCH_THRESHOLD     700
#define TOUCH_FILTER_VALUE  10
#define TOUCH_DELAY_MS      200

#define LED_GPIO_PIN  GPIO_NUM_2

void controleLuzEsp(bool acao)
{
  esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN );
  gpio_set_direction(LED_GPIO_PIN , GPIO_MODE_OUTPUT);
  if(acao)
    gpio_set_level(LED_GPIO_PIN , 1);
  else
    gpio_set_level(LED_GPIO_PIN , 0);
}

void touchTask(void *pvParameters) {
    touch_pad_intr_enable();
    while (1) {
        uint32_t pad_status = touch_pad_get_status();
        if (pad_status & (1 << TOUCH_PAD_NUM0)) {
            controleLuzEsp(true);
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


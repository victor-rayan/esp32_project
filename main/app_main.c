#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp32/rom/uart.h"
#include <driver/gpio.h>
#include "driver/rtc_io.h"
#include "luzQuarto.h"
#include "driver/touch_pad.h"

#include "wifi.h"
#include "mqtt.h"

#define TOUCH_THRESHOLD     700
#define TOUCH_FILTER_VALUE  10
#define TOUCH_DELAY_MS      200

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while(true)
    {
       float temperatura = 20.0 + (float)rand()/(float)(RAND_MAX/10.0);
       sprintf(mensagem, "{\"potencia\": 22}");
       mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

       vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void touch_task(void *pvParameters) {
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

void app_main(void) {
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_NUM0, TOUCH_THRESHOLD);
    touch_pad_filter_start(TOUCH_FILTER_VALUE);
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();
    // ledConfig();
    

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    // xTaskCreate(&ligaBotao, "Liga Botao", 4096, NULL, 1, NULL);
    xTaskCreate(&touch_task, "touch_task", 2048, NULL, 5, NULL);
}

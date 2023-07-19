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
#include "ligaTela.h"
#include "sdkconfig.h"
#include "dht11.h"
#include "rotary_encoder.h"

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

void app_main(void) {
    
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
    ledConfig();
    

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    // xTaskCreate(&ligaBotao, "Liga Botao", 4096, NULL, 1, NULL);
    // initTouch();
    // xTaskCreate(&touchTask, "touchTask", 2048, NULL, 5, NULL);
    // xTaskCreate(&DHT11Task, "DHTTask", 2048, NULL, 1, NULL);
    xTaskCreate(&encoder_task, "encoderTask", 2048, NULL, 1, NULL);
}

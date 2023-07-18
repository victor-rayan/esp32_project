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
#include "bpm_sensor.h"

#include "wifi.h"
#include "mqtt.h"
#include "ligaTela.h"
#include "ports.h"

int heartbeat = 0;

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
      //  float temperatura = 20.0 + (float)rand()/(float)(RAND_MAX/10.0);
       sprintf(mensagem, "{\"potencia\": 22}");
       mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

       vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}


void initHeartbeatRoutine(void * params)
{
  setup(HEARTBEAT_SENSOR);
  monitorBPM();
}

void app_main(void) {
    
   
    xTaskCreate(&initHeartbeatRoutine, "Monitoramento BPM", 8192, NULL, 1, NULL);
}

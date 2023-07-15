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

#define BOTAO 0
#define LED 2

#include "wifi.h"
#include "mqtt.h"

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

void luzEsp(bool acao)
{
  esp_rom_gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);
  if(acao)
    gpio_set_level(LED, 1);
  else
    gpio_set_level(LED, 0);
}

void ligaBotao()
{
    esp_rom_gpio_pad_select_gpio(BOTAO);
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);

    bool botaoPressionado = false;
    bool funcionalidadeHabilitada = false;

    while (true)
    {
      if (gpio_get_level(BOTAO) == 0)
      {
        if (!botaoPressionado)
        {      
          printf("Botão de enable pressionado\n");
          botaoPressionado = true;

          funcionalidadeHabilitada = !funcionalidadeHabilitada;

          if (funcionalidadeHabilitada)
          {
            printf("Funcionalidade habilitada\n");
            luzEsp(true);
          }
          else
          {
            printf("Funcionalidade desabilitada\n");
            luzEsp(false);
          }
        }
      }
      else
      {
        botaoPressionado = false;
      }
      vTaskDelay(pdMS_TO_TICKS(10));  
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


void app_main(void)
{
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
    

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&ligaBotao, "Liga Botao", 4096, NULL, 1, NULL);
}
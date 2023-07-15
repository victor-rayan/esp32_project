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
#include "driver/ledc.h"


#define BOTAO_GPIO_PIN   GPIO_NUM_0
#define LED_GPIO_PIN     GPIO_NUM_2

#define LED_RED_GPIO_PIN   GPIO_NUM_4
#define LED_GREEN_GPIO_PIN GPIO_NUM_16
#define LED_BLUE_GPIO_PIN  GPIO_NUM_15

#define LEDC_CHANNEL_RED   LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE  LEDC_CHANNEL_2

#define LEDC_TIMER         LEDC_TIMER_0
#define LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_BIT     LEDC_TIMER_8_BIT

#define LEDC_FREQ_HZ       5000
#define LEDC_RESOLUTION    LEDC_TIMER_8_BIT


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
  esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN);
  gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);
  if(acao)
    gpio_set_level(LED_GPIO_PIN, 1);
  else
    gpio_set_level(LED_GPIO_PIN, 0);
}

void ledConfig()
{
  esp_rom_gpio_pad_select_gpio(LED_RED_GPIO_PIN);
  esp_rom_gpio_pad_select_gpio(LED_GREEN_GPIO_PIN);
  esp_rom_gpio_pad_select_gpio(LED_BLUE_GPIO_PIN);

  gpio_set_direction(LED_RED_GPIO_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_GREEN_GPIO_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_BLUE_GPIO_PIN, GPIO_MODE_OUTPUT);

  ledc_timer_config_t timerConfig = {
      .duty_resolution = LEDC_RESOLUTION,
      .freq_hz = LEDC_FREQ_HZ,
      .speed_mode = LEDC_MODE,
      .timer_num = LEDC_TIMER
  };
  ledc_timer_config(&timerConfig);
  ledc_channel_config_t channelConfig = {
      .gpio_num = LED_RED_GPIO_PIN,
      .speed_mode = LEDC_MODE,
      .channel = LEDC_CHANNEL_RED,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0
  };
  ledc_channel_config(&channelConfig);
  channelConfig.gpio_num = LED_GREEN_GPIO_PIN;
  channelConfig.channel = LEDC_CHANNEL_GREEN;
  ledc_channel_config(&channelConfig);

  channelConfig.gpio_num = LED_BLUE_GPIO_PIN;
  channelConfig.channel = LEDC_CHANNEL_BLUE;
  ledc_channel_config(&channelConfig);

  ledc_fade_func_install(0);
}

void setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, r);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, g);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, b);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE);
}

void ligaBotao()
{
    esp_rom_gpio_pad_select_gpio(BOTAO_GPIO_PIN);
    gpio_set_direction(BOTAO_GPIO_PIN, GPIO_MODE_INPUT);

    bool botaoPressionado = false;
    int cont = 0;

    while (true)
    {
      if (gpio_get_level(BOTAO_GPIO_PIN) == 0)
      {
        if (!botaoPressionado)
        {      
          botaoPressionado = true;

          cont++;

          if (cont == 1) setRGB(50,205,153); // Aquamarine Médio
          if (cont == 2) setRGB(107,35,142); // Azul Ardósia Escuro
          if (cont == 3) setRGB(92,51,23); // Baker's Chocolate
          if (cont == 4) setRGB(255,165,0); // Laranja
          if (cont == 5) setRGB(255,0,255); // Magenta
          if (cont == 6) setRGB(230,232,250); // Silver
          if (cont == 7) setRGB(35,142,35); // Verde Floresta
          if (cont == 8) setRGB(255,0,0); // Vermelho
          if (cont == 9){
            setRGB(0,0,0);
            cont = 0;
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
    ledConfig();
    

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&ligaBotao, "Liga Botao", 4096, NULL, 1, NULL);
}
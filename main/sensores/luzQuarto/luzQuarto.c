#include "luzQuarto.h"
#include <driver/gpio.h>
#include "driver/rtc_io.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "json_parser.c"

#define BOTAO_GPIO_PIN   GPIO_NUM_0
#define LED_GPIO_PIN     GPIO_NUM_2

#define LED_RED_GPIO_PIN   GPIO_NUM_16
#define LED_GREEN_GPIO_PIN GPIO_NUM_17
#define LED_BLUE_GPIO_PIN  GPIO_NUM_15

#define LEDC_CHANNEL_RED   LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE  LEDC_CHANNEL_2

#define LEDC_TIMER         LEDC_TIMER_0
#define LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_BIT     LEDC_TIMER_8_BIT

#define LEDC_FREQ_HZ       5000
#define LEDC_RESOLUTION    LEDC_TIMER_8_BIT

void ledConfig() {
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

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, r);
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, g);
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, b);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE);
}

void setaCor() {
  switch(corLed) {
    case RED:
      setRGB(255, 0, 0); // Vermelho
      break;
    case BLUE:
      setRGB(0, 0, 255); // Azul
      break;
    case GREEN:
      setRGB(0, 255, 0); // Verde
      break;
    case YELLOW:
      setRGB(255, 255, 0); // Amarelo
      break;
    case MAGENTA:
      setRGB(255, 0, 255); // Magenta
      break;
    case CHOCOLATE:
      setRGB(92, 51, 23); // Baker's Chocolate
      break;
    case ARDOSIA:
      setRGB(107, 35, 142); // Azul Ardósia Escuro
      break;
    case FLORESTA:
      setRGB(35, 142, 35); // Verde Floresta
      break;
    default:
      setRGB(0, 0, 0);
      break;
  }
}

void ligaBotao() {
  bool botaoPressionado = false;
  
  esp_rom_gpio_pad_select_gpio(BOTAO_GPIO_PIN);
  gpio_set_direction(BOTAO_GPIO_PIN, GPIO_MODE_INPUT);

  while (true) {
    if (gpio_get_level(BOTAO_GPIO_PIN) == 0) {
      if (!botaoPressionado) {      
        botaoPressionado = true;
        setaCor();
      }
    }
    else
    {
      botaoPressionado = false;
    }
    vTaskDelay(pdMS_TO_TICKS(10));  
  }
}

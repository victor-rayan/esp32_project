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
#include "rotary_encoder.h"
#include <stdio.h>
#include "rotary_encoder.h"

#define ROT_ENC_A_GPIO 25 // Pino GPIO para sinal A do encoder
#define ROT_ENC_B_GPIO 27 // Pino GPIO para sinal B do encoder
#define ENABLE_HALF_STEPS true // Ativar meio passo (opcional, conforme necessário)
#define MAX_TEMPERATURE 100 // Temperatura máxima permitida (exemplo)

// Variáveis globais para armazenar informações do encoder
// Definir função MAX para obter o valor máximo entre dois números
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

rotary_encoder_info_t info;
int32_t rotaryPosition;
int32_t referenceTemperature;

// Definir função MAX para obter o valor máximo entre dois números
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Função para ajustar a temperatura do sistema com base na referência
void adjustTemperature() {
    // Implemente aqui o código para ajustar o sistema de acordo com a referenceTemperature.
    // Por exemplo, ligue ou desligue um sistema de aquecimento/resfriamento, ajuste a velocidade do ventilador, etc.
    printf("Ajustando a temperatura para: %ld\n", referenceTemperature);
}

void app_main(void) {
    // esp32-rotary-encoder requer que o serviço de ISR do GPIO seja instalado antes de chamar rotary_encoder_register()
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Inicializa o dispositivo encoder rotativo com os pinos GPIO para os sinais A e B
    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));

    // Cria uma fila para eventos do driver do encoder rotativo.
    // Tarefas podem ler desta fila para receber informações de posição atualizadas.
    QueueHandle_t event_queue = xQueueCreate(10, sizeof(rotary_encoder_event_t));
    info.state.position = 0;
    rotaryPosition = info.state.position;
    referenceTemperature = rotaryPosition * 5; // Cada clique do encoder ajusta a temperatura em 5 graus
    printf("Posição inicial: %ld\n", rotaryPosition);
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, (QueueHandle_t)event_queue));

    // Loop principal
    while (1) {
        // Aguarde um evento da fila do encoder rotativo
        rotary_encoder_event_t event;
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdTRUE) {
            // Evento recebido, atualize a posição do encoder e a temperatura de referência
            rotaryPosition += event.position;
            rotaryPosition = MAX(0, rotaryPosition); // Garante que a posição não seja negativa
            referenceTemperature = rotaryPosition * 5; // Cada clique do encoder ajusta a temperatura em 5 graus

            // Verifique os limites da temperatura
            if (referenceTemperature > MAX_TEMPERATURE) {
                referenceTemperature = MAX_TEMPERATURE;
            }

            // Atualiza o sistema para a nova temperatura de referência
            adjustTemperature();
            printf("Nova posição: %ld, Temperatura de referência: %ld\n", rotaryPosition, referenceTemperature);
        }
    }
}
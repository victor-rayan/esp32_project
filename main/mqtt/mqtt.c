#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "json_parser.h"
#include "luzQuarto.h"

#include "mqtt.h"

#define TAG "MQTT"

extern SemaphoreHandle_t conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

int systemON = 0;

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void parse_event_data(char *data) {   
    char json_message[100];
    switch (json_parse_return_comm(data)) {
    case TURN_SYSTEM:
        if (systemON == 0) {
            ESP_LOGI(TAG, "Turn on System");
            systemON = 1;
        } else {
            ESP_LOGI(TAG, "Turn off System");
            systemON = 0;
        }
        break;
    case TURN_ON_LIGHT:
        setaCor();
        break;
    case TURN_OFF_LIGHT:
        setRGB(0,0,0);
        break;
    case GREEN:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Verde");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = GREEN;
        break;
    case BLUE:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Azul");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = BLUE;
        break;
    case RED:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Vermelho");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = RED;
        break;
    case YELLOW:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Amarelo");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = YELLOW;
        break; 
    case ARDOSIA:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Ardosia");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = ARDOSIA;
        break;
    case MAGENTA:   
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Magenta");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = MAGENTA;
        break;
    case FLORESTA:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Floresta");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = FLORESTA;
        break;
    case CHOCOLATE:
        snprintf(json_message, sizeof(json_message), "{\"cor\": %s}", "Chocolate");
        mqtt_envia_mensagem(MQTT_ATTRIBUTES_PATH, json_message);
        corLed = CHOCOLATE;
        break;
    default:
        break;
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, (int) event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xSemaphoreGive(conexaoMQTTSemaphore);
        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        parse_event_data(event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_start() {
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtt://164.41.98.25",
        .credentials.username = "SU6LjX84paNYZPKyWeij"
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem) {
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mensagem enviada, ID: %d", message_id);
}
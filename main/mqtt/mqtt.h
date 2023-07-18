#ifndef MQTT_H
#define MQTT_H

#define MQTT_TELEMETRY_PATH "v1/devices/me/telemetry"
#define MQTT_ATTRIBUTES_PATH "v1/devices/me/attributes"

void mqtt_start();

extern int systemON;


void mqtt_envia_mensagem(char * topico, char * mensagem);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "json_parser.h"

#define TAG "JSON_PARSER"

int corLed;

int json_parse_return_comm(const char *const json) {
  if (strstr(json, "azul")) {
    corLed = BLUE;
    return BLUE;
  } else if (strstr(json, "verde")) {
    corLed = GREEN;
    return GREEN;
  } else if (strstr(json, "vermelho")) {
    corLed = RED;
    return RED;
  } else if (strstr(json, "amarelo")) {
    corLed = YELLOW;
    return YELLOW;
  } else if (strstr(json, "ardosia")) {
    corLed = ARDOSIA;
    printf("ardosia");
    return ARDOSIA;
  } else if (strstr(json, "magenta")) {
    corLed = MAGENTA;
    printf("magenta");
    return MAGENTA;
  } else if (strstr(json, "floresta")) {
    printf("floresta %s", json);
    corLed = FLORESTA;
    return FLORESTA;
  } else if (strstr(json, "chocolate")) {
    corLed = CHOCOLATE;
    printf("chocolate");
    return CHOCOLATE;
  } else if (strstr(json, "light")) {
    printf("LIGHT on");
    return TURN_ON_LIGHT;
  } else if (strstr(json, "off")) {
    printf("LIGHT of");
    return TURN_OFF_LIGHT;
  } else if (strstr(json, "on")) {
    printf("ligar/desligar %s", json);
    return TURN_SYSTEM;
  } 


  return -1;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "json_parser.h"

#define TAG "JSON_PARSER"

int corLed;

int json_parse_return_comm(const char *const json)
{
  if (strstr(json, "azul"))
  {
    corLed = BLUE;
    return BLUE;
  }

  if (strstr(json, "verde"))
  {
    corLed = GREEN;
    return GREEN;
  }

  if (strstr(json, "vermelho"))
  {
    corLed = RED;
    return RED;
  }

  if (strstr(json, "amarelo"))
  {
    corLed = YELLOW;
    return YELLOW;
  }

  if (strstr(json, "ardosia"))
  {
    corLed = ARDOSIA;
    printf("ardosia");
    return ARDOSIA;
  }

  if (strstr(json, "magenta"))
  {
    corLed = MAGENTA;
    printf("magenta");
    return MAGENTA;
  }

  if (strstr(json, "floresta"))
  {
    printf("floresta");
    corLed = FLORESTA;
    return FLORESTA;
  }

  if (strstr(json, "chocolate"))
  {
    corLed = CHOCOLATE;
    printf("chocolate");
    return CHOCOLATE;
  }

  if (strstr(json, "on"))
  {
    printf("ligar/desligar");
    return TURN_SYSTEM;
  }

  if (strstr(json, "true"))
  {
    printf("LIGHT");
    return TURN_ON_LIGHT;
  }

  if (strstr(json, "false"))
  {
    printf("LIGHT");
    return TURN_OFF_LIGHT;
  }



  return -1;
}
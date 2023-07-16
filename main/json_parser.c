#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "json_parser.h"

#define TAG "JSON_PARSER"

int json_parse_return_comm(const char *const json)
{
  if (strstr(json, "azul"))
  {
    printf("aqui no azul");
    return BLUE;
  }

  if (strstr(json, "verde"))
  {
    printf("aqui no verde");
    return GREEN;
  }

  if (strstr(json, "vermelho"))
  {
    printf("aqui no vermelho");
    return RED;
  }

  if (strstr(json, "amarelo"))
  {
    printf("aqui no amarelo");
    return YELLOW;
  }

  if (strstr(json, "on"))
  {
    printf("ligar/desligar");
    return TURN_SYSTEM;
  }



  return -1;
}
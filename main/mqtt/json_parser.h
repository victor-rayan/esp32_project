#ifndef JSON_PARSER_H
#define JSON_PARSER_H

extern int corLed;

#define TURN_SYSTEM 2
#define GREEN 4
#define RED 5
#define BLUE 6
#define YELLOW 7
#define ARDOSIA 8
#define MAGENTA 9
#define FLORESTA 10
#define CHOCOLATE 11

int json_parse_return_comm(const char * const json);

#endif
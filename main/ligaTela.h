#ifndef LIGA_TELA_H
#define LIGA_TELA_H

#include "driver/touch_pad.h"

#define TOUCH_THRESHOLD     700
#define TOUCH_FILTER_VALUE  10
#define TOUCH_DELAY_MS      200

void touchTask(void *pvParameters);
void initTouch();

#endif /* LIGA_TELA_H */

#ifndef HAL_WSLED_H
#define HAL_WSLED_H

#include "ZComDef.h"

// 初始化 WS2812B LED
void Hal_WsLed_Init(void);

// 设置 LED RGB 颜色
void Hal_WsLed_SetRGB(uint8 red, uint8 green, uint8 blue);

#endif

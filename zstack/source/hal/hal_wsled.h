#ifndef HAL_WSLED_H
#define HAL_WSLED_H

#include "hal_types.h"

// 初始化 WS2812B LED
void hal_wsled_init(void);

// 设置 LED RGB 颜色
void hal_wsled_setRgb(uint8 red, uint8 green, uint8 blue);

// 设置色调、饱和度
void hal_wsled_setHueSat(uint8 dimmer, uint8 hue, uint8 sat);

// 设置色温
void hal_wsled_setColorTemp(uint8 dimmer, uint16 colorTemp);

#endif

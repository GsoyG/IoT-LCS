#ifndef HAL_WSLED_H
#define HAL_WSLED_H

#include "hal_types.h"

// 初始化 WS2812B LED
void hal_wsled_init(void);

// 设置 LED RGB 颜色
void hal_wsled_setRgb(uint8 red, uint8 green, uint8 blue);

#endif

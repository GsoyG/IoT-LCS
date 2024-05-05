#ifndef HAL_HDC1080_H
#define HAL_HDC1080_H

#include "ZComDef.h"

#define HDC_1080_ADD            0x40
#define CONFIGURATION_REG_ADD   0x02
#define TEMPERATURE_REG_ADD     0x00
#define HUMIDITY_REG_ADD        0x01

typedef enum {
    Temperature_Resolution_14_bit = 0,
    Temperature_Resolution_11_bit = 1
} Temp_Reso;

typedef enum {
    Humidity_Resolution_14_bit = 0,
    Humidity_Resolution_11_bit = 1,
    Humidity_Resolution_8_bit = 2
} Humi_Reso;

void hal_hdc1080_init(Temp_Reso Temperature_Resolution_x_bit, Humi_Reso Humidity_Resolution_x_bit);
uint8 hal_hdc1080_measurement(float* temperature, uint8* humidity);
#endif

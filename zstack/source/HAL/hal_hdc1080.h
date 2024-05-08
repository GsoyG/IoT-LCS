#ifndef HAL_HDC1080_H
#define HAL_HDC1080_H

#include "hal_types.h"

#define HDC_1080_ADD            0x80
#define TEMPERATURE_REG_ADD     0x00
#define HUMIDITY_REG_ADD        0x01
#define CONFIGURATION_REG_ADD   0x02

typedef enum {
    Temperature_Resolution_14_bit = 0,
    Temperature_Resolution_11_bit = 1
} Temp_Reso;

typedef enum {
    Humidity_Resolution_14_bit = 0,
    Humidity_Resolution_11_bit = 1,
    Humidity_Resolution_8_bit = 2
} Humi_Reso;

void hal_hdc1080_init(Temp_Reso temperature_resolution, Humi_Reso humidity_resolution);
void hal_hdc1080_measurement(uint16* temperature, uint16* humidity);
#endif

#ifndef HAL_HDC1080_H
#define HAL_HDC1080_H

#include "hal_types.h"

#define HDC1080_DEFAULT_TEMPERATURE_RESO TEMPERATURE_RESOLUTION_14_BIT
#define HDC1080_DEFAULT_HUMIDITY_RESO HUMIDITY_RESOLUTION_14_BIT

typedef enum {
    TEMPERATURE_RESOLUTION_14_BIT = 0,
    TEMPERATURE_RESOLUTION_11_BIT = 1
} hdc1080_temp_reso;

typedef enum {
    HUMIDITY_RESOLUTION_14_BIT = 0,
    HUMIDITY_RESOLUTION_11_BIT = 1,
    HUMIDITY_RESOLUTION_8_BIT = 2
} hdc1080_humi_reso;

void hal_hdc1080_init(void);

void hal_hdc1080_init_ex(hdc1080_temp_reso temperature_resolution, hdc1080_humi_reso humidity_resolution);

void hal_hdc1080_measurement(uint16* temperature, uint16* humidity);

#endif // HAL_HDC1080_H

#ifndef BH1750_H
#define BH1750_H

#include "hal_types.h"

#define BH1750FVI_DEFAULT_RESOLUTION_MODE BH1750FVI_MODE_HIGH_RESOLUTION_MODE
#define BH1750FVI_DEFAULT_MEASUREMENT_TIME 69

typedef enum {
    BH1750FVI_MODE_HIGH_RESOLUTION_MODE = 0, // < 1lx resolution
    BH1750FVI_MODE_HIGH_RESOLUTION_MODE2 = 1, // < 0.5lx resolution
    BH1750FVI_MODE_LOW_RESOLUTION_MODE = 2, // < 4lx resolution
} bh1750_reso_mode;

void hal_bh1750_init(void);

void hal_bh1750_init_ex(bh1750_reso_mode resolution_mode, uint8 measurement_time);

// 1 to on, 0 to off
void hal_bh1750_power(uint8 on_off);

void hal_bh1750_set_mode(bh1750_reso_mode resolution_mode);

void hal_bh1750_set_time(uint8 measurement_time);

void hal_bh1750_measurement(uint16* illuminance);

#endif // BH1750_H

#ifndef BH1750_H
#define BH1750_H

#include "hal_types.h"

#define BH1750FVI_DEFAULT_MODE BH1750FVI_MODE_HIGH_RESOLUTION_MODE
#define BH1750FVI_DEFAULT_MEASUREMENT_TIME 69

typedef enum {
    BH1750FVI_MODE_HIGH_RESOLUTION_MODE = 0x0,        /**< 1lx resolution */
    BH1750FVI_MODE_HIGH_RESOLUTION_MODE2 = 0x1,        /**< 0.5lx resolution */
    BH1750FVI_MODE_LOW_RESOLUTION_MODE = 0x2,        /**< 4lx resolution */
} bh1750_reso_mode;

// Initializes BH1750 sensor with resolution mode.
void hal_bh1750_init(void);

void hal_bh1750_init_ex(bh1750_reso_mode resolution_mode, uint8 measurement_time);

void hal_bh1750_power_on(void);

void hal_bh1750_power_down(void);

void hal_bh1750_set_mode(bh1750_reso_mode resolution_mode);

void hal_bh1750_set_measurement_time(uint8 measurement_time);

// Reads the light level from BH1750 sensor.
void hal_bh1750_measurement(uint16* illuminance);

#endif // BH1750_H

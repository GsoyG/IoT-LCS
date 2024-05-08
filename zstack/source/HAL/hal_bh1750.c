#include "hal_bh1750.h"
#include "hal_i2c.h"
#include "hal_delay.h"

#define BH1750FVI_ADDRESS                                   0x23
#define BH1750FVI_COMMAND_POWER_DOWN                        0x00 /**< power down command */
#define BH1750FVI_COMMAND_POWER_ON                          0x01 /**< power on command */
#define BH1750FVI_COMMAND_RESET                             0x07 /**< reset command */
#define BH1750FVI_COMMAND_CONTINUOUSLY_H_RESOLUTION_MODE    0x10 /**< continuously h-resolution mode command */
#define BH1750FVI_COMMAND_CONTINUOUSLY_H_RESOLUTION_MODE2   0x11 /**< continuously h-resolution mode2 command */
#define BH1750FVI_COMMAND_CONTINUOUSLY_L_RESOLUTION_MODE    0x13 /**< continuously l-resolution mode command */
#define BH1750FVI_COMMAND_ONE_TIME_H_RESOLUTION_MODE        0x20 /**< one time h-resolution mode command */
#define BH1750FVI_COMMAND_ONE_TIME_H_RESOLUTION_MODE2       0x21 /**< one time h-resolution mode2 command */
#define BH1750FVI_COMMAND_ONE_TIME_L_RESOLUTION_MODE        0x23 /**< one time l-resolution mode command */
#define BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_HIGH      0x40 /**< change measurement time high command */
#define BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_LOW       0x60 /**< change measurement time low command */

static uint8 bh1750_resolution_mode, bh1750_measurement_time;

void hal_bh1750_init(void) {
    uint8 send_data;

    send_data = BH1750FVI_COMMAND_POWER_ON;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    hal_delay(20);

    send_data = BH1750FVI_COMMAND_RESET;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    hal_delay(20);

    send_data = BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_HIGH | ((69 >> 5) & 0x07);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    send_data = BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_LOW | ((69 >> 0) & 0x1F);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    hal_delay(20);
}

void hal_bh1750_init_ex(bh1750_reso_mode resolution_mode, uint8 measurement_time) {
    hal_bh1750_init();
    hal_bh1750_power_on();
    hal_bh1750_set_mode(resolution_mode);
    hal_bh1750_set_measurement_time(measurement_time);
}

void hal_bh1750_power_on(void) {
    uint8 send_data = BH1750FVI_COMMAND_POWER_ON;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
}

void hal_bh1750_power_down(void) {
    uint8 send_data = BH1750FVI_COMMAND_POWER_DOWN;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
}

void hal_bh1750_set_mode(bh1750_reso_mode resolution_mode) {
    bh1750_resolution_mode = resolution_mode;
}

void hal_bh1750_set_measurement_time(uint8 measurement_time) {
    uint8 send_data;

    send_data = BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_HIGH | ((measurement_time >> 5) & 0x07);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    send_data = BH1750FVI_COMMAND_CHANGE_MEASUREMENT_TIME_LOW | ((measurement_time >> 0) & 0x1F);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    bh1750_measurement_time = measurement_time;
}

void hal_bh1750_measurement(uint16* illuminance) {
    uint8 send_data;

    if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE) /* high resolution mode */
    {
        send_data = BH1750FVI_COMMAND_ONE_TIME_H_RESOLUTION_MODE; /* high resolution mode command */
    }
    else if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE2) /* high resolution mode2 */
    {
        send_data = BH1750FVI_COMMAND_ONE_TIME_H_RESOLUTION_MODE2; /* high resolution mode2 command */
    }
    else if (bh1750_resolution_mode == BH1750FVI_MODE_LOW_RESOLUTION_MODE) /* low resolution mode */
    {
        send_data = BH1750FVI_COMMAND_ONE_TIME_L_RESOLUTION_MODE; /* low resolution mode command */
    }
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE) /* high resolution mode */
    {
        hal_delay((uint16)(180.0f * (float)(bh1750_measurement_time) / 69.0f) * 3); /* delay the max time */
    }
    else if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE2) /* high resolution mode2 */
    {
        hal_delay((uint16)(180.0f * (float)(bh1750_measurement_time) / 69.0f) * 3); /* delay the max time */
    }
    else /* low resolution mode */
    {
        hal_delay((uint16)(24.0f * (float)(bh1750_measurement_time) / 69.0f) * 3); /* delay the max time */
    }
    uint8 receive_data[2];
    hal_i2c_receive(BH1750FVI_ADDRESS, receive_data, 2);

    float lux;
    uint16 raw = ((receive_data[0] << 8) | receive_data[1]); /* get the raw data */
    if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE) /* high resolution mode */
    {
        lux = (float)(raw) / 1.2f * (69.0f / ((float)(bh1750_measurement_time))); /* convert */
    }
    else if (bh1750_resolution_mode == BH1750FVI_MODE_HIGH_RESOLUTION_MODE2) /* high resolution mode2 */
    {
        lux = (float)(raw) / 1.2f * (69.0f / ((float)(bh1750_measurement_time))) / 2.0f; /* convert */
    }
    else /* low resolution mode */
    {
        lux = (float)(raw) / 1.2f * (69.0f / ((float)(bh1750_measurement_time))); /* convert */
    }

    // Convert the two bytes to a lux value
    *illuminance = (uint16)raw;
}

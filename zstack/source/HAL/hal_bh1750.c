#include "hal_bh1750.h"
#include "hal_i2c.h"
#include "hal_delay.h"

#define BH1750FVI_ADDRESS                               0x23

#define BH1750FVI_CMD_POWER_DOWN                        0x00 /**< power down command */
#define BH1750FVI_CMD_POWER_ON                          0x01 /**< power on command */
#define BH1750FVI_CMD_RESET                             0x07 /**< reset command */
#define BH1750FVI_CMD_CONTINUOUSLY_H_RESOLUTION_MODE    0x10 /**< continuously h-resolution mode command */
#define BH1750FVI_CMD_CONTINUOUSLY_H_RESOLUTION_MODE2   0x11 /**< continuously h-resolution mode2 command */
#define BH1750FVI_CMD_CONTINUOUSLY_L_RESOLUTION_MODE    0x13 /**< continuously l-resolution mode command */
#define BH1750FVI_CMD_ONE_TIME_H_RESOLUTION_MODE        0x20 /**< one time h-resolution mode command */
#define BH1750FVI_CMD_ONE_TIME_H_RESOLUTION_MODE2       0x21 /**< one time h-resolution mode2 command */
#define BH1750FVI_CMD_ONE_TIME_L_RESOLUTION_MODE        0x23 /**< one time l-resolution mode command */
#define BH1750FVI_CMD_CHANGE_MEASUREMENT_TIME_HIGH      0x40 /**< change measurement time high command */
#define BH1750FVI_CMD_CHANGE_MEASUREMENT_TIME_LOW       0x60 /**< change measurement time low command */

static uint8 bh1750_resolution_mode, bh1750_measurement_time;

void hal_bh1750_init(void) {
    hal_bh1750_init_ex(BH1750FVI_DEFAULT_RESOLUTION_MODE, BH1750FVI_DEFAULT_MEASUREMENT_TIME);
}

void hal_bh1750_init_ex(bh1750_reso_mode resolution_mode, uint8 measurement_time) {
    uint8 send_data;

    // power on
    send_data = BH1750FVI_CMD_POWER_ON;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    hal_delay(5);

    // reset
    send_data = BH1750FVI_CMD_RESET;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
    hal_delay(5);

    // set resolution mode and measurement time
    hal_bh1750_set_mode(resolution_mode);
    hal_bh1750_set_time(measurement_time);
    hal_delay(5);
}

void hal_bh1750_power(uint8 on_off) {
    uint8 send_data = on_off ? BH1750FVI_CMD_POWER_ON : BH1750FVI_CMD_POWER_DOWN;
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);
}

void hal_bh1750_set_mode(bh1750_reso_mode resolution_mode) {
    bh1750_resolution_mode = resolution_mode;
}

void hal_bh1750_set_time(uint8 measurement_time) {
    uint8 send_data;

    // set high byte
    send_data = BH1750FVI_CMD_CHANGE_MEASUREMENT_TIME_HIGH | ((measurement_time >> 5) & 0x07);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    // set low byte
    send_data = BH1750FVI_CMD_CHANGE_MEASUREMENT_TIME_LOW | ((measurement_time >> 0) & 0x1F);
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    bh1750_measurement_time = measurement_time;
}

void hal_bh1750_measurement(uint16* illuminance) {
    uint8 send_data;

    // set resolution mode
    switch (bh1750_resolution_mode) {
    default:
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE:
        send_data = BH1750FVI_CMD_ONE_TIME_H_RESOLUTION_MODE;
        break;
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE2:
        send_data = BH1750FVI_CMD_ONE_TIME_H_RESOLUTION_MODE2;
        break;
    case BH1750FVI_MODE_LOW_RESOLUTION_MODE:
        send_data = BH1750FVI_CMD_ONE_TIME_L_RESOLUTION_MODE;
        break;
    }
    hal_i2c_transmit(BH1750FVI_ADDRESS, &send_data, 1);

    // wait for measurement
    switch (bh1750_resolution_mode) {
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE:
        hal_delay((uint16)((float)bh1750_measurement_time / 69.0f * 180.0f));
        break;
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE2:
        hal_delay((uint16)((float)bh1750_measurement_time / 69.0f * 180.0f));
        break;
    default:
        hal_delay((uint16)((float)bh1750_measurement_time / 69.0f * 24.0f));
        break;
    }

    // receive data
    uint8 receive_data[2];
    hal_i2c_receive(BH1750FVI_ADDRESS, receive_data, 2);
    uint16 raw = ((receive_data[0] << 8) | receive_data[1]);

    // convert to lux
    uint16 lux;
    switch (bh1750_resolution_mode) {
    default:
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE:
        lux = (uint16)((float)raw / 1.2f * (69.0f / (float)bh1750_measurement_time));
        break;
    case BH1750FVI_MODE_HIGH_RESOLUTION_MODE2:
        lux = (uint16)((float)raw / 1.2f * (69.0f / (float)bh1750_measurement_time) / 2.0f);
        break;
    }

    *illuminance = lux;
}

#include "hal_hdc1080.h"
#include "hal_i2c.h"
#include "hal_delay.h"

#define HDC1080_ADDRESS                 0x40

#define HDC1080_TEMPERATURE_REG_ADD     0x00
#define HDC1080_HUMIDITY_REG_ADD        0x01
#define HDC1080_CONFIGURATION_REG_ADD   0x02

void hal_hdc1080_init(void) {
    hal_hdc1080_init_ex(TEMPERATURE_RESOLUTION_14_BIT, HUMIDITY_RESOLUTION_14_BIT);
}

void hal_hdc1080_init_ex(hdc1080_temp_reso temperature_resolution, hdc1080_humi_reso humidity_resolution) {
    /* Temperature and Humidity are acquired in sequence, Temperature first
     * Default: Temperature resolution = 14 bit, Humidity resolution = 14 bit */

    // Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1
    uint16 config_reg_value = 0x1000;
    if (temperature_resolution == TEMPERATURE_RESOLUTION_11_BIT)
        config_reg_value |= (1 << 10); //11 bit
    switch (humidity_resolution) {
    case HUMIDITY_RESOLUTION_11_BIT:
        config_reg_value |= (1 << 8);
        break;
    case HUMIDITY_RESOLUTION_8_BIT:
        config_reg_value |= (1 << 9);
        break;
    }

    uint8 send_data[2];
    send_data[0] = (config_reg_value >> 8);
    send_data[1] = (config_reg_value & 0x00ff);
    hal_i2c_write(HDC1080_ADDRESS, HDC1080_CONFIGURATION_REG_ADD, send_data, 2);
}


void hal_hdc1080_measurement(uint16* temperature, uint16* humidity) {
    // Send measurement command
    uint8 send_data = HDC1080_TEMPERATURE_REG_ADD;
    hal_i2c_transmit(HDC1080_ADDRESS, &send_data, 1);

    /* Delay here 15ms for conversion compelete.
     * Note: datasheet say maximum is 7ms, but when delay=7ms, the read value is not correct
     */
    hal_delay(30);

    uint8 receive_data[4];
    hal_i2c_receive(HDC1080_ADDRESS, receive_data, 4);

    uint16 raw[2];
    raw[0] = ((receive_data[0] << 8) | receive_data[1]);
    raw[1] = ((receive_data[2] << 8) | receive_data[3]);

    // convert temperature and humidity
    uint16 temp, humi;
    temp = (uint16)(raw[0] / 65536.0f * 165.0f - 40.0);
    humi = (uint16)(raw[1] / 65536.0f * 100.0f);

    *temperature = temp * 100;
    *humidity = humi * 100;
}

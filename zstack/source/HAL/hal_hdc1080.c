#include "hal_hdc1080.h"
#include "hal_i2c.h"
#include "hal_delay.h"

void hal_hdc1080_init(Temp_Reso Temperature_Resolution_x_bit, Humi_Reso Humidity_Resolution_x_bit) {
    /* Temperature and Humidity are acquired in sequence, Temperature first
     * Default:   Temperature resolution = 14 bit,
     *            Humidity resolution = 14 bit
     */

     /* Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1 */
    uint16 config_reg_value = 0x1000;
    uint8 data_send[2];

    if (Temperature_Resolution_x_bit == Temperature_Resolution_11_bit) {
        config_reg_value |= (1 << 10); //11 bit
    }

    switch (Humidity_Resolution_x_bit) {
    case Humidity_Resolution_11_bit:
        config_reg_value |= (1 << 8);
        break;
    case Humidity_Resolution_8_bit:
        config_reg_value |= (1 << 9);
        break;
    }

    data_send[0] = (config_reg_value >> 8);
    data_send[1] = (config_reg_value & 0x00ff);

    hal_i2c_write(HDC_1080_ADD << 1, Configuration_register_add, data_send, 2);
}


uint8 hal_hdc1080_measurement(float* temperature, uint8* humidity) {
    uint8 receive_data[4];
    uint16 temp_x, humi_x;
    uint8 send_data = Temperature_register_add;

    hal_i2c_transmit(HDC_1080_ADD << 1, &send_data, 1);

    /* Delay here 15ms for conversion compelete.
     * Note: datasheet say maximum is 7ms, but when delay=7ms, the read value is not correct
     */
    hal_delay(15);

    /* Read temperature and humidity */
    hal_i2c_receive(HDC_1080_ADD << 1, receive_data, 4);

    temp_x = ((receive_data[0] << 8) | receive_data[1]);
    humi_x = ((receive_data[2] << 8) | receive_data[3]);

    *temperature = ((temp_x / 65536.0) * 165.0) - 40.0;
    *humidity = (uint8)((humi_x / 65536.0) * 100.0);

    return 0;

}

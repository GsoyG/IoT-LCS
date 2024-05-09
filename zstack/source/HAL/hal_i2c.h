#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal_types.h"

extern void hal_i2c_init(void);

extern void hal_i2c_write(uint8 slaveAddr, uint8 regAddr, uint8 *data, uint8 length);

extern void hal_i2c_read(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length);

extern bool hal_i2c_transmit(uint8 slaveAddr, uint8* data, uint8 length);

extern void hal_i2c_receive(uint8 slaveAddr, uint8* data, uint8 length);

#endif

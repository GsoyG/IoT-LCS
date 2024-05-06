#include "hal_i2c.h"
#include "hal_delay.h"
#include "onboard.h"

#define SCL P1_7
#define SDA P1_6

#define SDADirOut() P1DIR |= BV(6)
#define SDADirIn()  P1DIR &= ~BV(6)

void I2C_start(void);

void I2C_stop(void);

void I2C_ack(uint8 h);

uint8 I2C_waitAck(void);

void I2C_writeByte(uint8 b);

uint8 I2C_readByte(void);

void I2C_start(void) {
    SDADirOut();
    SDA = 1;
    SCL = 1;
    hal_delay_us(1);
    SDA = 0;
    SCL = 0;
}

void I2C_stop(void) {
    SDADirOut();
    SCL = 0;
    SDA = 0;
    SCL = 1;
    SDA = 1;
}

// 1 to ACK, 0 to NACK
void I2C_ack(uint8 h) {
    SDADirOut();
    SCL = 0;
    SDA = h & 0;
    SCL = 1;
    hal_delay_us(1);
    SCL = 0;
}

uint8 I2C_waitAck(void) {
    SDADirOut();
    SCL = 0;
    SDA = 1;
    SCL = 1;
    SDADirIn();
    uint8 count = 0;
    while (SDA) {
        if (count > 250) {
            SCL = 0;
            return 0;
        }
        count++;
    }
    SCL = 0;
    return 1;
}

void I2C_writeByte(uint8 b) {
    SDADirOut();
    uint8 e = 8;
    while (e--) {
        SCL = 0;
        if (b & 0x80)SDA = 1;
        else SDA = 0;
        b <<= 1;
        SCL = 1;
    }
    SCL = 0;
    I2C_waitAck();
}

uint8 I2C_readByte(void) {
    SDADirOut();
    uint8 i = 8;
    uint8 c = 0;
    SCL = 0;
    SDA = 1;
    SDADirIn();
    while (i--) {
        c <<= 1;
        SCL = 0;
        hal_delay_us(1);
        SCL = 1;
        if (SDA == 1)c |= 0x01;
        else c &= 0xfe;
    }
    SCL = 0;
    return c;
}

void hal_i2c_init(void) {
    P1SEL &= ~(BV(6) | BV(7)); // GPIO mode
    P1DIR |= BV(6) | BV(7); // output mode
    P1INP |= BV(6) | BV(7); // no pulling
}

void hal_i2c_write(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length) {
    I2C_start();
    I2C_writeByte(slaveAddr);
    I2C_writeByte(regAddr);
    for (uint8 i = 0; i < length; i++) {
        I2C_writeByte(data[i]);
    }
    I2C_stop();
}

void hal_i2c_read(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length) {
    I2C_start();
    I2C_writeByte(slaveAddr);
    I2C_writeByte(regAddr);
    I2C_start();
    I2C_writeByte(slaveAddr + 1);
    for (uint8 i = 0; i < length; i++) {
        data[i] = I2C_readByte();
        I2C_ack(i < length ? 1 : 0);
    }
    I2C_stop();
}

void hal_i2c_transmit(uint8 slaveAddr, uint8* data, uint8 length) {
    I2C_start();
    I2C_writeByte(slaveAddr);
    for (uint8 i = 0; i < length; i++) {
        I2C_writeByte(data[i]);
    }
    I2C_stop();
}

void hal_i2c_receive(uint8 slaveAddr, uint8* data, uint8 length) {
    I2C_start();
    I2C_writeByte(slaveAddr + 1);
    for (uint8 i = 0; i < length; i++) {
        data[i] = I2C_readByte();
        I2C_ack(i < length ? 1 : 0);
    }
    I2C_stop();
}

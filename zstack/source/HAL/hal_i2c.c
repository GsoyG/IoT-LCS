#include "hal_i2c.h"
#include "hal_delay.h"
#include "onboard.h"

#define SCL P1_7
#define SDA P1_6

#define SCLDirOut() P1DIR |= 0x04
#define SDADirOut() P1DIR |= 0x08
#define SDADirIn()  P1DIR &= ~0x08

void I2C_Start(void);

void I2C_Stop(void);

void I2C_Ack(uint8 h);

uint8 I2C_Check_Ack(void);

void I2C_Write_Byte(uint8 b);

uint8 I2C_Read_Byte(void);

void I2C_Start(void) {
    SDA = 1;
    SCL = 1;
    hal_delay_us(1);
    SDA = 0;
    SCL = 0;
}

void I2C_Stop(void) {
    SCL = 0;
    SDA = 0;
    SCL = 1;
    SDA = 1;
}

void I2C_Ack(uint8 h) {
    SCL = 0;
    SDA = h & 0x01;
    SCL = 1;
    hal_delay_us(1);
    SCL = 0;
}

uint8 I2C_Check_Ack(void) {
    P1DIR |= BV(6);
    SCL = 0;
    SDA = 1;
    SCL = 1;
    if (SDA == 1) {
        SCL = 0;
        return 0;
    }
    SCL = 0;
    return 1;
}

void I2C_Write_Byte(uint8 b) {
    uint8 e = 8;
    while (e--) {
        SCL = 0;
        if (b & 0x80)SDA = 1;
        else SDA = 0;
        b <<= 1;
        SCL = 1;
    }
    SCL = 0;
    I2C_Check_Ack();
}

uint8 I2C_Read_Byte(void) {
    uint8 i = 8;
    uint8 c = 0;
    SCL = 0;
    SDA = 1;
    P1DIR &= ~BV(6); // input mode
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
    I2C_Start();
    I2C_Write_Byte(slaveAddr);
    I2C_Write_Byte(regAddr);
    for (uint8 i = 0; i < length; i++) {
        I2C_Write_Byte(data[i]);
    }
    I2C_Stop();
}

void hal_i2c_read(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length) {
    I2C_Start();
    I2C_Write_Byte(slaveAddr);
    I2C_Write_Byte(regAddr);
    I2C_Start();
    I2C_Write_Byte(slaveAddr + 1);
    for (uint8 i = 0; i < length; i++) {
        data[i] = I2C_Read_Byte();
        I2C_Ack(i < length ? 1 : 0);
    }
    I2C_Stop();
}

void hal_i2c_transmit(uint8 slaveAddr, uint8* data, uint8 length) {
    I2C_Start();
    I2C_Write_Byte(slaveAddr);
    for (uint8 i = 0; i < length; i++) {
        I2C_Write_Byte(data[i]);
    }
    I2C_Stop();
}

void hal_i2c_receive(uint8 slaveAddr, uint8* data, uint8 length) {
    I2C_Start();
    I2C_Write_Byte(slaveAddr + 1);
    for (uint8 i = 0; i < length; i++) {
        data[i] = I2C_Read_Byte();
        I2C_Ack(i < length ? 1 : 0);
    }
    I2C_Stop();
}

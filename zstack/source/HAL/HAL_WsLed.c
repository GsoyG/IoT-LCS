#include "HAL_WsLed.h"
#include "onboard.h"

#define LED_SBIT P1_5

// bufferSize = 24 bits * 3 bits per bit / 8 bit
#define WS2812_bufferSize 9

uint8 WS2812_buffer[WS2812_bufferSize];
uint8 WS2812_bit = 0;
uint16 WS2812_byte = 0;

// 将位存入缓冲区
void WS2812_StoreBit(uint8 bit);
// 将颜色存入缓冲区
void WS2812_StoreColor(uint8 color);

void WS2812_StoreBit(uint8 bit) {
    if (bit) WS2812_buffer[WS2812_byte] |= 1 << WS2812_bit;
    WS2812_bit++;

    // 检测进位
    if (WS2812_bit > 7) {
        WS2812_bit = 0;
        WS2812_byte++;
    }
}

void WS2812_StoreColor(uint8 color) {
    for (uint8 i = 8; i; i--) {
        if ((color >> (i - 1)) & 1) {
            WS2812_StoreBit(1);
            WS2812_StoreBit(1);
            WS2812_StoreBit(0);
        }
        else {
            WS2812_StoreBit(1);
            WS2812_StoreBit(0);
            WS2812_StoreBit(0);
        }
    }
}

void Hal_WsLed_SetRGB(uint8 red, uint8 green, uint8 blue) {
    WS2812_bit = 0;
    WS2812_byte = 0;

    // 清除缓冲区
    for (uint16 i = 0; i < WS2812_bufferSize; i++)
        WS2812_buffer[i] = 0;

    WS2812_StoreColor(green);
    WS2812_StoreColor(red);
    WS2812_StoreColor(blue);

    for (uint16 j = 0; j < WS2812_bufferSize; j++) {
        U0DBUF = WS2812_buffer[j];
        while ((U0CSR & (1 << 0))); // 等待发送完成
    }
}

void Hal_WsLed_Init(void) {
    P1SEL |= BV(5);  // 0=GPIO 1=Peripheral (SPI)
    P1DIR |= BV(5);  // 1=output
    P1INP = 0x00;    // 1=no pulling

    PERCFG |= BV(0); // UART0 SPI Alternative #2 Pins
    U0CSR = 0x00;    // UART0 SPI Master
    U0GCR = 0x10;    // UART0 Baud_E
    U0BAUD = 0x40;   // UART0 Baud_M

    Hal_WsLed_SetRGB(0, 0, 0);
}

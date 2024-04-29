#include "HAL_WsLed.h"
#include "ZComDef.h"
#include "onboard.h"

#define LED_SBIT P1_5

// buffersize = 24 bits * 3 bits per bit / 8 bit
#define WS2812_buffersize 9

uint8 WS2812_buffer[WS2812_buffersize];
uint8 WS2812_bit = 0;
uint16 WS2812_byte = 0;

void WS2812_StoreBit(uint8 bit);
void WS2812_SendLED(uint8 red, uint8 green, uint8 blue);

void WS2812_StoreBit(uint8 bit) {
    if (bit) WS2812_buffer[WS2812_byte] |= 1 << WS2812_bit;
    WS2812_bit++;

    if (WS2812_bit > 7) {
        WS2812_bit = 0;
        WS2812_byte++;
    }
}

void WS2812_SendLED(uint8 red, uint8 green, uint8 blue) {
    WS2812_bit = 0;
    WS2812_byte = 0;

    for (uint16 i = 0; i < WS2812_buffersize; i++) WS2812_buffer[i] = 0;

    uint8 i;
    for (i = 8; i; i--) {
        if ((green >> (i - 1)) & 1) {
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
    for (i = 8; i; i--) {
        if ((red >> (i - 1)) & 1) {
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
    for (i = 8; i; i--) {
        if ((blue >> (i - 1)) & 1) {
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

    for (uint16 j = 0; j < WS2812_buffersize; j++) {
        U1DBUF = WS2812_buffer[j];
        while ((U1CSR & (1 << 0))); // wait until byte is sent
    }
}

void HalWsLedInit(void) {
    P1SEL |= BV(5); // 0=GPIO 1=Peripheral (SPI)
    P1DIR |= BV(5); // 1=output

    U1CSR = 0x00; // UART1 SPI Master
    U1GCR = 0x10; // UART1 Baud_E
    U1BAUD = 0x40; // UART1 Baud_M

    WS2812_SendLED(0, 0, 0);
}

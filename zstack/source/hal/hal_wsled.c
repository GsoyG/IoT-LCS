#include "hal_wsled.h"
#include "onboard.h"

#define LED_SBIT P1_5

// bufferSize = 24 bits * 3 bits per bit / 8 bit
#define WS2812_bufferSize 9

uint8 WS2812_buffer[WS2812_bufferSize];
uint8 WS2812_bit = 0;
uint16 WS2812_byte = 0;

// 将位存入缓冲区
void WS2812_storeBit(uint8 bit);
// 将颜色存入缓冲区
void WS2812_storeColor(uint8 color);

// 辅助函数，将色调转换为RGB分量
float hue_to_rgb(float p, float q, float t);
// 将HSL颜色转换为RGB颜色
void hsl_to_rgb(float h, float s, float l, uint8* r, uint8* g, uint8* b);


float hue_to_rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0 / 6) return p + (q - p) * 6 * t;
    if (t < 1.0 / 2) return q;
    if (t < 2.0 / 3) return p + (q - p) * (2.0 / 3 - t) * 6;
    return p;
}
void hsl_to_rgb(float h, float s, float l, uint8* r, uint8* g, uint8* b) {
    if (s == 0) *r = *g = *b = (uint8)l; // 饱和度为0时，颜色为灰色，直接设置为明度
    else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        *r = (uint8)(255 * hue_to_rgb(p, q, h + 1.0 / 3));
        *g = (uint8)(255 * hue_to_rgb(p, q, h));
        *b = (uint8)(255 * hue_to_rgb(p, q, h - 1.0 / 3));
    }
}

void WS2812_storeBit(uint8 bit) {
    if (bit) WS2812_buffer[WS2812_byte] |= 1 << WS2812_bit;
    WS2812_bit++;

    // 检测进位
    if (WS2812_bit > 7) {
        WS2812_bit = 0;
        WS2812_byte++;
    }
}

void WS2812_storeColor(uint8 color) {
    for (uint8 i = 8; i; i--) {
        if ((color >> (i - 1)) & 1) {
            WS2812_storeBit(1);
            WS2812_storeBit(1);
            WS2812_storeBit(0);
        }
        else {
            WS2812_storeBit(1);
            WS2812_storeBit(0);
            WS2812_storeBit(0);
        }
    }
}

void hal_wsled_setRgb(uint8 red, uint8 green, uint8 blue) {
    WS2812_bit = 0;
    WS2812_byte = 0;

    // 清除缓冲区
    for (uint16 i = 0; i < WS2812_bufferSize; i++)
        WS2812_buffer[i] = 0;

    WS2812_storeColor(green);
    WS2812_storeColor(red);
    WS2812_storeColor(blue);

    for (uint16 j = 0; j < WS2812_bufferSize; j++) {
        U0DBUF = WS2812_buffer[j];
        while ((U0CSR & (1 << 0))); // 等待发送完成
    }
}

void hal_wsled_setHueSat(uint8 hue, uint8 sat) {
    uint8 rgb[3] = { 0, 0, 0 };
    hsl_to_rgb((float)hue / 254.0f, 1.0f, 1.0f - ((float)sat / 254.0f) / 2.0f, rgb, rgb + 1, rgb + 2);
    hal_wsled_setRgb(rgb[0], rgb[1], rgb[2]);
}

void hal_wsled_setColorTemp(uint16 colorTemp) {
    uint8 rgb[3] = { 0, 0, 0 };
    hsl_to_rgb(0.083f, 1.0f, 1.0f - ((float)colorTemp / 500.0f) * 0.4f, rgb, rgb + 1, rgb + 2);
    hal_wsled_setRgb(rgb[0], rgb[1], rgb[2]);
}

void hal_wsled_setBrightness(uint8 brightness) {
    hal_wsled_setRgb(brightness, brightness, brightness);
}

void hal_wsled_init(void) {
    P1SEL |= BV(5);  // 0=GPIO 1=Peripheral (SPI)
    P1DIR |= BV(5);  // 1=output
    P1INP &= ~BV(5);  // 1=no pulling

    PERCFG |= BV(0); // UART0 SPI Alternative #2 Pins
    U0CSR = 0x00;    // UART0 SPI Master
    U0GCR = 0x10;    // UART0 Baud_E
    U0BAUD = 0x40;   // UART0 Baud_M

    hal_wsled_setRgb(0, 0, 0);
}

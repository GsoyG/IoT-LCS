#include "hal_delay.h"

void hal_delay(uint16 ms) {
    while (ms--)
        hal_delay_us(1000);
}

void hal_delay_us(uint16 us) {
    while (us--)
        asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
}

#include "gpio.h"
#include "system_timer.h"
#include "uart.h"

void blink_once()
{
    // Turn on GPIO 24
    gpio[GPSET0] |= 1 << 24;

    timer_delay_ms(500);

    // Turn off GPIO 24
    gpio[GPCLR0] |= 1 << 24;

    timer_delay_ms(500);
}

void blink_code(uint32_t err)
{
    for(int i = 0; i < err; ++i)
    {
        blink_once();
    }

    // Only delay 4 seconds, since we delay for 1 additional
    // second in blink_once().
    timer_delay_ms(4500);
}

int main()
{
    init_uart();
    gpio[GPFSEL1] |= (1 << 27);
    gpio[GPFSEL2] |= (1 << 12);

    while(1){
        
    }
    
    return 0;
}

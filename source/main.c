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
        // generate the mastermind codeword
            // char [4] codeword;
            // void mastermind(codeword);
        // prompt the user for their first guess
            // while{print_string("Take a Guess\r\n>> ")
            // char [5] guess
            // get_string(guess, 5);
        // compare the user's guess against the mastermind's codeword
            // if guess[i] == codeword[i]...; else if guess[i] == codeword[0..3]...;
        // illuminate or blink the required LED's
            // if correct solid(); else if incorrect, but is somewhere else blink(); else nothing;
        // prompt the user for their next guess
            // print_string("\r\n")}
        // repeat 10 times or until the user guesses the correct codeword
    }
    
    return 0;
}

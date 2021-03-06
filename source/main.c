#include "gpio.h"
#include "system_timer.h"
#include "uart.h"

//Blink all the LEDs to make sure they are ready.
void blink_once()
{
    // Turn on GPIO 24
    gpio[GPSET0] |= 1 << 17;
	timer_delay_ms(500);
    gpio[GPSET0] |= 1 << 23;
	timer_delay_ms(500);
    gpio[GPSET0] |= 1 << 19;
	timer_delay_ms(500);
    gpio[GPSET0] |= 1 << 16;

    timer_delay_ms(500);

    // Turn off GPIO 24
    gpio[GPCLR0] |= 1 << 17;
    gpio[GPCLR0] |= 1 << 23;
    gpio[GPCLR0] |= 1 << 19;
    gpio[GPCLR0] |= 1 << 16;

    timer_delay_ms(500);
}

//Used for debugging
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

//Flash the amount of LEDs corresponding to the user's answers
void LEDFlash(int correct, int wrongLocation) {
	//The offset from GPIO 24 where everything was tested
	int GPIO_offset[4] = {-7, -1, -5, -8};
	
	//Turn on all the correct ones
	for (int i = 0; i < correct; i++) {
		gpio[GPSET0] |= 1 << (24 + GPIO_offset[i]);
	}
	
	//Blink all the correct but wrong location
	for (int counter = 0; counter < 10; counter++) {
		for (int i = correct; i < wrongLocation + correct; i++) {
			if ((counter & 1) > 0) {
				gpio[GPSET0] |= 1 << (24 + GPIO_offset[i]);
			} else {
				gpio[GPCLR0] |= 1 << (24 + GPIO_offset[i]);
			}
		}
		timer_delay_ms(200);
	}
	//Shut everything off
	for (int i = 0; i < 4; i++) {
		gpio[GPCLR0] |= 1 << (24 + GPIO_offset[i]);
	}
}


//Current state of the random
uint32_t state = 100;


//Modulo function
uint32_t mod(uint32_t dividend, uint32_t divisor) {
    while (dividend > divisor) {
        dividend -= divisor;
    }

    return dividend;
}


//Generates a random integer with a minimum value of min and a maximum value of max.
//Range is inclusive
uint32_t random(uint32_t min, uint32_t max) {
    max = max + 1;
    uint32_t counter = min;
    min = min * 100;
    max = max * 100;

    state = state ^ (state << 13);
    state = state ^ (state >> 17);
    state = state ^ (state << 5);

    uint32_t tmp = mod(state, max);
    while (tmp < min) {
        tmp += min;
        tmp = mod(tmp, max);
    }

    for (uint32_t i = min; i < max; i += 100) {
        if (tmp < i + 100) {
            return counter;
        }
        counter++;
    }

    return counter;
}


int main()
{
	//Seed the randomizer
	state = sys_timer[SYS_TIMER_CLO];
	
	//Initialize the UART
    init_uart();
    gpio[GPFSEL1] |= 0x1000 << 9;
    gpio[GPFSEL2] |= 0x200;
	gpio[GPFSEL1] |= 0x1000 << 6;
	gpio[GPFSEL1] |= 0x1000 << 15;
	blink_once();
	

    // generate the mastermind codeword
    // char [4] codeword;
    char codeword[4] = {0,0,0,0};
    char options[6] = {'1','2','3','4','5','6'};

    //Loop through and generate random codeword
    while (codeword[0] == 0 || codeword[1] == 0 || codeword[2] == 0 || codeword[3] == 0) {
        char newLetter = options[random(0, 5)];

        for (int i = 0; i < 4; i++) {
            if (codeword[i] == 0) {
                codeword[i] = newLetter;
                break;
            }
            if (codeword[i] == newLetter) {
                break;
            }
        }
    }

    int roundCount = 0;
    // prompt the user for their first guess
    while (1) {
        roundCount += 1;
        put_string("Round ");
		if (roundCount < 10) {
			put_char(roundCount + '0');
		} else {
			put_string("10");
		}
        put_string(": Take a Guess\r\n>> ");
        char guess[5] = {0,0,0,0};
        get_string(guess, 5);

        int correctCount = 0;  //Number of correct choices
        int wrongLocationCount = 0;   //Number of correct choices that are in the wrong location

        for (int i = 0; i < 4; i++) {
            //Check if the pin is in the right location
            if (guess[i] == codeword[i]) {
                correctCount += 1;
            }

            //Check if the correct pins are on but in the wrong location
            for (int c = 0; c < 4; c++) {
                if (i != c && guess[i] == codeword[c]) {
                    wrongLocationCount += 1;
                }
            }
        }

        //Show how close the user is via LED
        LEDFlash(correctCount, wrongLocationCount);

		put_string("\r\n");
        put_char(correctCount + '0');
        put_string(" were in the correct location. ");
        put_char(wrongLocationCount + '0');
        put_string(" were the correct choice but the wrong location.\r\n");

        if (correctCount == 4) {
            put_string("You win!\r\n");
            break;
        }
        if (roundCount == 10) {
            put_string("Game over.  Correct answer was: ");
            for (int i = 0; i < 4; i++) {
				put_char(codeword[i]);
			}
            break;
        }


    }


    return 0;
}

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL  // Define CPU frequency as 16 MHz if not already defined
#endif
/*
Target: 16,000 cycles per ms
Cycles per iteration: 19
Perfect count: 16,000 / 19 ≈ 842 iterations
*/
void ft_delay_ms(uint16_t ms)
{
    while (ms--)
    {
        for (volatile uint16_t i = 0; i < 843; i++)
            ;
    }
    
}

int main()
{
    DDRB |= (1 << 1);  // Configure PB1 as OUTPUT to control LED D2 DDRB: Data Direction Register for Port Setting bit 1 high = OUTPUT mode
    while (1)
    {
        PINB = (1 << PB1);  // Toggle LED D2 state
        ft_delay_ms(20000);   // Delay to make the toggle visible
    }
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main()
{
    DDRD |= (1 << 3) | (1 << 5) | (1 << 6);  // Configure PD3 as OUTPUT to control LED D3
    while (1)
    {
        PORTD = (PORTD & ~((1 << 3) | (1 << 6))) | (1 << 5);  // Toggle LED D5 red state
        _delay_ms(1000);   // Delay to make the toggle visible
        PORTD = (PORTD & ~((1 << 3) | (1 << 5))) | (1 << 6);  // Toggle LED D5 green state
        _delay_ms(1000);   // Delay to make the toggle visible
        PORTD = (PORTD & ~((1 << 5) | (1 << 6))) | (1 << 3);  // Toggle LED D5 blue state
        _delay_ms(1000);   // Delay to make the toggle visible
    }
}
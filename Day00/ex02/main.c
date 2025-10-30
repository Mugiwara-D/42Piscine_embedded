#include <avr/io.h>

int main()
{
    DDRB |= (1 << 0); // Set PB0 as output

    PORTD |= (1 << 2); // enable pull-up resistor on PD0

    while (1)
    {
        if (PIND & (1 << 2)) // check if PD0 is high
        {
            PORTB &= ~(1 << 0); // Set PB0 low (turn off LED)
        }
        else
        {
            PORTB |= (1 << 0); // Set PB0 high (turn on LED)
        }
    }
    return 0;
}
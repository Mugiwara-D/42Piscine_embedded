#include <avr/io.h>

int main()
{
    DDRB |= (1 << 0); // Set PB0 as output
    PORTB |= (1 << 0); // Set PB0 high (turn on LED)
    while (1)
    {
        /* code */
    }
    return 0;
}
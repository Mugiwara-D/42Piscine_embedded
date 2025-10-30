#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 115200
#endif
/*
UBRR = (F_CPU / (16 * BAUD)) - 1
UBRR = (16000000 / (16 * 125000)) - 1
UBRR = 7.680

**If UBRR = 8:**
Actual Baud = F_CPU / (16 * (UBRR + 1))
            = 16,000,000 / (16 * 9)
            = 16,000,000 / 144
            = 111,111 baud
Error: (111,111 - 115,200) / 115,200 = -3.5%

**If UBRR = 7:**
Actual Baud = 16,000,000 / (16 * 8)
            = 16,000,000 / 128
            = 125,000 baud
Error: (125,000 - 115,200) / 115,200 = +8.5%

UART typically tolerates up to ±5% error. So UBRR = 8 is the better choice (only -3.5% error).
*/

void    UART_init(unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter
    UCSR0B = (1 << 3);  // Enable transmitter (bit 3)
    // Set frame format: 8data, 1stop bit
    UCSR0C = (1 << 2) | (1 << 1);
}

int main()
{

}
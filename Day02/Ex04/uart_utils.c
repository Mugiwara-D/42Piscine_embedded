#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void    uart_init(unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter
    UCSR0B = (1 << 7) | (1 << 4) | (1 << 3);  // RXCIE0 + RXEN0 + TXEN0
    //           ↑         ↑         ↑
    //      RX Interrupt  RX Enable  TX Enable
    // Set frame format: 8data, 1stop bit
    UCSR0C = (1 << 2) | (1 << 1);
}

char uart_rx()
{
    // Wait for data to be received
    while (!(UCSR0A & (1 << 7)));
    // Get and return received data from buffer
    return UDR0;
}

void    uart_tx(unsigned char c)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << 5)));
    // Put data into buffer, sends the data
    UDR0 = c;
}

void uart_printstr(const char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

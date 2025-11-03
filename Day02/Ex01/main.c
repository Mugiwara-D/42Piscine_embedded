#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void    uart_init(unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter
    UCSR0B = (1 << 3);  // Enable transmitter (bit 3)
    // Set frame format: 8data, 1stop bit
    UCSR0C = (1 << 2) | (1 << 1);
}

void timer_init(void)
{
    // Your setup code but:
    // - Remove DDRB line (no LED needed)
    // - Remove COM1A0 bit (no pin toggle needed)
    TCCR1A = 0;  // Normal port operation
    
    TCCR1B = (1 << 3) | (1 << 2) | (1 << 0);  // CTC mode, prescaler 1024
    
    // For 2 seconds: OCR1A = (16MHz / (1024 * 0.5Hz)) - 1
    OCR1A = 31249;  // Calculate this!
    
    // Enable Timer1 Compare A interrupt
    TIMSK1 = (1 << 1);  // OCIE1A bit
    
    // Enable global interrupts
    sei();
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

ISR(TIMER1_COMPA_vect)
{
    uart_printstr("Hello, World!\n\r");
}

int main()
{
    uart_init(8); // UBRR value for 115200 baud with F_CPU = 16MHz
    timer_init();

    while (1)
    {
    }
    return 0;
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL   
#define BAUD 115200
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void    uart_init(unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter
    UCSR0B = (1 << 7) | (1 << 4) | (1 << 3);  // RXCIE0 + RXEN0 + TXEN0
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

void uart_print(const char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

void process_backspace(char *input_buffer, uint8_t *buffer_index)
{
    if (*buffer_index > 0 && *buffer_index <= 7)
    {
        *buffer_index -= 1;
        input_buffer[*buffer_index] = '\0';
        // Move cursor back, overwrite with space, move cursor back again
        uart_print("\b \b");
    }

}



uint8_t parse_hex_digit(char c) 
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

int check_buffer_full(char *buffer, uint8_t index)
{
    for (uint8_t i = 0; i < index; i++)
    {
        if ((buffer[i] < '0' || buffer[i] > '9') && (buffer[i] < 'A' ||
            buffer[i] > 'F') && (buffer[i] < 'a' || buffer[i] > 'f'))
            return 0;
    }
    return 1;
}

int main() 
{
    uart_init(8);
    init_rgb();
    
    uart_print("RGB LED Controller Ready!\r\n");  
    uart_print("Send colors as #RRGGBB\r\n");
    
    char buffer[8];
    uint8_t index = 0;
    
    while (1) 
    {
  
    }
}
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
        *buffer_index--;
        input_buffer[*buffer_index] = '\0';
        // Move cursor back, overwrite with space, move cursor back again
        uart_print("\b \b");
    }

}

void init_rgb()
{
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) 
{
    OCR0B = r;
    OCR0A = g;
    OCR2B = b;
}

uint8_t parse_hex_digit(char c) 
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

uint8_t get_color_value(char high, char low) 
{
    return (parse_hex_digit(high) << 4) | parse_hex_digit(low);
}

int main() 
{
    uart_init(8);
    init_rgb();
    
    uart_print("RGB LED Controller Ready!\r\n");  
    uart_print("Send colors as #RRGGBB\r\n");
    
    char buffer[8];
    uint8_t index = 0;
    
    while (1) {
        char c = uart_rx();
        uart_tx(c);  
        
        if (c == '\n' || c == '\r') 
        {
            buffer[index] = '\0';  
            
            if (index == 7 && buffer[0] == '#')
            {
                set_rgb(
                    get_color_value(buffer[1], buffer[2]),
                    get_color_value(buffer[3], buffer[4]),
                    get_color_value(buffer[5], buffer[6])
                );
                uart_print(" -> Color set!\r\n");  
            } 
            else if (index > 0)
                uart_print(" -> Invalid format!\r\n");
            index = 0;
        }
        else if (c == '\b' || c == 0x7F)   // Backspace or DEL
            process_backspace(buffer, &index); 
        else if (index < 7)
            buffer[index++] = c;
    }
}
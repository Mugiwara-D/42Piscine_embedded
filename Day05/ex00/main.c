#include <avr/io.h>
#include <util/delay.h>

void    uart_init(unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    UCSR0B = (1 << 3);  // Enable transmitter (bit 3)
    
    UCSR0C = (1 << 2) | (1 << 1); // Set frame format: 8data, 1stop bit
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

void    print_hex(uint16_t byte)
{
    const char hex_chars[] = "0123456789abcdef";
    uart_tx(hex_chars[(byte >> 4) & 0x0F]); // Print high nibble byte / 16
    uart_tx(hex_chars[byte & 0x0F]); // Print low nibble byte % 16
}


int main(void) 
{
    uart_init(8); // Initialize UART with baud rate 115200 for 16MHz clock
    ADMUX = 0b01100000; // Select ADC0 (PC0) as input channel and ADLAR = 1 for 8-bit resolution
    ADCSRA = 0b10000111; // Enable ADC and set prescaler to 128
    while (1) 
    {
        ADCSRA |= (1 << ADSC); // Start conversion
        while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
        uint16_t adc_value = ADCH; // Read ADC value
        uart_printstr("ADC Value: ");
        print_hex(adc_value);
        uart_printstr("\r\n");
        _delay_ms(20); // Wait for 1 second
        

    }
}
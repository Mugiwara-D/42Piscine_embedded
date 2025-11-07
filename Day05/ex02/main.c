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

void    print_decimal(uint16_t byte)
{
    if (byte >= 100)
    {
        uart_tx('0' + (byte / 100));
        byte %= 100;
        uart_tx('0' + (byte / 10));
        byte %= 10;
        uart_tx('0' + byte);
    }
    else if (byte >= 10)
    {
        uart_tx('0' + (byte / 10));
        byte %= 10;
        uart_tx('0' + byte);
    }
    else
    {
        uart_tx('0' + byte);
    }
}

uint16_t read_adc(uint8_t channel)
{
    ADMUX = (ADMUX & 0b11110000) | (channel & 0b00001111); // Select ADC channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    uint16_t lower_byte = ADCL; // Read low byte
    uint16_t high_byte = ADCH; // Read high byte
    return lower_byte | (high_byte << 8); // Read ADC value
}

int main(void) 
{
    uart_init(8); // Initialize UART with baud rate 115200 for 16MHz clock
    ADMUX = 0b01000000; // Select ADC0 (PC0) as input channel and ADLAR = 1 for 8-bit resolution
    ADCSRA = 0b10000111; // Enable ADC and set prescaler to 128
    while (1) 
    {
        uint16_t pot_value = read_adc(0); // Read ADC_pot value from ADC0
        uint16_t ldr_value = read_adc(1); // Read ADC_ldr value from ADC1
        uint16_t ntc_value = read_adc(2); // Read ADC_ntc value from ADC2
        uart_printstr("ADC pot, ldr, ntc Value: ");
        print_decimal(pot_value);
        uart_printstr(", ");
        print_decimal(ldr_value);
        uart_printstr(", ");
        print_decimal(ntc_value);
        uart_printstr("\r\n");
        _delay_ms(20); // Wait for 1 second
        

    }
}
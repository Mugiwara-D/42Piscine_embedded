#include <avr/io.h>
#include <avr/eeprom.h>

#define BAUD_RATE 115200
#define UBRR_VALUE 8 
//((F_CPU / (16UL * BAUD_RATE)) - 1)

void uart_init(void)
{
    // Set baud rate
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    
    // Enable transmitter
    UCSR0B = (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for buffer empty
    UDR0 = c;
}

void uart_printstr(const char *str)
{
    while (*str)
        uart_tx(*str++);
}

void uart_printhex(uint8_t value)
{
    char hex[] = "0123456789abcdef";
    uart_tx(hex[value >> 4]);    // High nibble
    uart_tx(hex[value & 0x0F]);  // Low nibble
}

void    uart_printaddr(uint16_t addr)
{
    uart_printstr("000");
    uart_printhex((addr >> 8) & 0xFF); // High byte
    uart_printhex(addr & 0xFF);        // Low byte
    uart_printstr(" ");
}

int main(void)
{
    uart_init();
    uart_printstr("EEPROM Test Start\r\n");
    for (uint16_t i = 0; i < 1024; i++)
    { 
        uint8_t byte = eeprom_read_byte((uint8_t*)i);
        if (i % 16 == 0)
            uart_printaddr(i);
        uart_printhex(byte);
        uart_printstr(" ");
        if ((i + 1) % 16 == 0)
            uart_printstr("\r\n");
    }
    while (1);
}
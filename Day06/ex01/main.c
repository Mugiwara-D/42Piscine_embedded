#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
/*
Mask to get just the status bits (ignore prescaler bits)
#define TW_STATUS_MASK 0xF8  // 0b11111000

uint8_t status = TWSR & TW_STATUS_MASK;
*/

#define BAUD_RATE 115200
#define UBRR_VALUE 8 
#define AHT20_ADDR 0x38
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
    char hex[] = "0123456789ABCDEF";
    uart_tx(hex[value >> 4]);    // High nibble
    uart_tx(hex[value & 0x0F]);  // Low nibble
}

void i2c_init(void)
{
    TWBR = 72; // Set bit rate
    TWSR = 0; // Prescaler value
    TWCR = (1 << TWEN); // Enable TWI
}

void i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition Bit:     7      6      5      4      3      2      1      0
                                                      //                            [TWINT] [TWEA] [TWSTA][TWSTO][TWWC] [TWEN]  [ - ] [TWIE]
    
    while (!(TWCR & (1 << TWINT))); // Wait for TWINT flag by checking if TWINT bit is set
    
    // if ((TWSR & 0xF8) != TW_START) 
    //     Handle error
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send STOP condition Bit:     7      6      5      4      3      2      1      0
                                                      //                           [TWINT] [TWEA] [TWSTA][TWSTO][TWWC] [TWEN]  [ - ] [TWIE]
    while (TWCR & (1 << TWSTO)); // Wait until STOP condition is executed and bus released
}

void i2c_write(unsigned char data)
{
    TWDR = data;                          // Load data
    TWCR = (1 << TWINT) | (1 << TWEN);    // Start transmission
    while (!(TWCR & (1 << TWINT)));       // Wait for completion
}

void i2c_read(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable TWI, generation of ACK
    while (!(TWCR & (1 << TWINT)));                   // Wait for completion
}

void i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN); // Enable TWI, no ACK generation
    while (!(TWCR & (1 << TWINT)));     // Wait for completion
}

void print_hex_value(char c)
{
    const char hex_chars[] = "0123456789ABCDEF";
    uart_tx(hex_chars[(c >> 4) & 0x0F]); // High nibble
    uart_tx(hex_chars[c & 0x0F]);        // Low nibble
}

int main(void)
{
    uart_init();
    i2c_init();
    uint8_t data[7];
    
    while (1)
    {
        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 0); // Write address with write bit
        i2c_write(0xAC); // Command byte
        i2c_write(0x33); // Data byte 1
        i2c_write(0x00); // Data byte 2
        i2c_stop();

        // Wait for measurement to complete 80ms (section 5.4 ATH20 datasheet)
        _delay_ms(80);

        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 1); // Write address with read bit
        
        for (int i = 0; i < 6; i++)
        {
            i2c_read();
            data[i] = TWDR;
        }
        i2c_read_nack(); // Read last byte without ACK 
        data[6] = TWDR;
        i2c_stop();

        for (int i = 0; i < 7; i++)
        {
            uart_printhex(data[i]);
            uart_tx(' ');
        }
        uart_printstr("\r\n");
        _delay_ms(1000); // Wait before next measurement  prevent self heatingz
        
    }
}
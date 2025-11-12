#include <avr/io.h>
#include <util/twi.h>
/*
Mask to get just the status bits (ignore prescaler bits)
#define TW_STATUS_MASK 0xF8  // 0b11111000

uint8_t status = TWSR & TW_STATUS_MASK;
*/

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

int main(void)
{
    uart_init();
    i2c_init();
    
    uart_printstr("=== I2C Communication Test with AHT20 ===\r\n\r\n");
    
    // Test 1: START condition
    uart_printstr("1. Sending START condition...\r\n");
    i2c_start();
    uart_printstr("   Status: 0x");
    uart_printhex(TWSR & 0xF8);
    uart_printstr(" (expected 0x08)\r\n\r\n");
    
    // Test 2: Send address + Write bit
    uart_printstr("2. Sending AHT20 address (0x38) + Write bit...\r\n");
    i2c_write((0x38 << 1) | 0); // AHT20 address with Write bit
    uart_printstr("   Status: 0x");
    uart_printhex(TWSR & 0xF8);
    uart_printstr(" (expected 0x18 if sensor responds)\r\n\r\n");
    
    // Test 3: STOP condition
    uart_printstr("3. Sending STOP condition...\r\n");
    i2c_stop();
    uart_printstr("   Done!\r\n\r\n");
    
    uart_printstr("=== Test Complete ===\r\n");
    
    while (1);
}
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdlib.h>
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

unsigned char i2c_read(unsigned char ack)
{
    if (ack)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable TWI, generation of ACK
    else
        TWCR = (1 << TWINT) | (1 << TWEN); // Enable TWI, no ACK
    
    while (!(TWCR & (1 << TWINT)));    // Wait for completion
    return TWDR;
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
    float temperature = 0, humidity = 0;
    float temp_history[3];
    float hum_history[3];
    int history_index = 0;
    int history_count = 0;                       
    int sample = 0;
    float temp_avg = 0, hum_avg = 0;
    char temp_str[10];
    char hum_str[10];


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
            data[i] = i2c_read(1); // Read byte with ACK
        data[6] = i2c_read(0); // Read last byte without ACK
        i2c_stop();
        
        uint32_t humidity_raw = ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | ((uint32_t)data[3]);  // Byte 1 → bits 19-12 | Byte 2 → bits 11-4 | Byte 3 upper half → bits 3-0
        humidity_raw = humidity_raw >> 4; // Shift to get 20 bits only
        uint32_t temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | ((uint32_t)data[5]);// Byte 3 lower half → bits 19-16 | Byte 4 → bits 15-8 | Byte 5 → bits 7-0

        humidity = (humidity_raw * 100.0) / 1048576.0; // RH% = (Srh / 2^20) * 100 = (Srh * 100) / 1048576
        temperature = (temp_raw * 200.0) / 1048576.0 - 50.0; // T = (St / 2^20) * 200 - 50 = (St * 200) / 1048576 - 50

        temp_history[history_index] = temperature;
        hum_history[history_index] = humidity;

        // Store in history
        temp_history[history_index] = temperature;
        hum_history[history_index] = humidity;
        sample = (history_count < 3) ? (history_count + 1) : 3;
        temp_avg = 0;
        hum_avg = 0;
        for (int i = 0; i < sample; i++)
        {
            temp_avg += temp_history[i];
            hum_avg += hum_history[i];
        }
        temp_avg /= sample;
        hum_avg /= sample;

        dtostrf(temp_avg, 6, 2, temp_str);
        dtostrf(hum_avg, 6, 2, hum_str);
        uart_printstr("Temperature: ");
        uart_printstr(temp_str);
        uart_printstr(" °C, Humidity: ");
        uart_printstr(hum_str);
        uart_printstr(" %\r\n");
        history_index = (history_index + 1) % 3;
        history_count++;
        _delay_ms(1000); // Wait before next measurement  prevent self heatingz
        
    }
}
/*
section 6 ATH20 datasheet
Humidity and Temperature Calculation
RH% = (Srh / 2^20) * 100 = (Srh * 100) / 1048576
T = (St / 2^20) * 200 - 50 = (St * 200) / 1048576 - 50
*/
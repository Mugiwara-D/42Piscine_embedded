#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Formula from datasheet:
// UBRR = (F_CPU / (16 * BAUD)) - 1
// For 9600 baud: UBRR = (16000000 / (16 * 9600)) - 1 = 103

#define BAUD 115200
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init() {
    UBRR0 = UBRR_VALUE;        // Set baud rate
    UCSR0B = (1 << RXEN0);     // Enable receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8 data bits, 1 stop bit
}

void init_rgb() {
    // Set pins as outputs
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
    
    // Timer 0 configuration
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    //        ^^^^^^^^^^^^   ^^^^^^^^^^^^     ^^^^^^^^^^   ^^^^^^^^^^
    //        Non-inverting  Non-inverting    Fast PWM     Fast PWM
    //        for OC0A       for OC0B         Mode 3       Mode 3
    //        (Green)        (Red)
    
    TCCR0B = (1 << CS00);  // No prescaler (clk/1)
    
    // Timer 2 configuration (same pattern for Blue)
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    //        ^^^^^^^^^^^^    ^^^^^^^^^^   ^^^^^^^^^^
    //        Non-inverting   Fast PWM     Fast PWM
    //        for OC2B        Mode 3       Mode 3
    //        (Blue)
    
    TCCR2B = (1 << CS20);  // No prescaler (clk/1)
}

char uart_rx(void) {
    // Wait for data to be received (check RXC0 flag in UCSR0A)
    while (!(UCSR0A & (1 << RXC0)));
    
    // Get and return received data from buffer
    return UDR0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    // Just write to the Output Compare Registers!
    OCR0B = r;   // Red
    OCR0A = g;   // Green  
    OCR2B = b;   // Blue
}

uint8_t parse_hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';        // '0'-'9' → 0-9
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;  // 'A'-'F' → 10-15
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;  // 'a'-'f' → 10-15
    return 0;
}

int get_color_value(char high, char low) {
    return (parse_hex_digit(high) << 4) | parse_hex_digit(low);
}

int main() {
    uart_init();
    init_rgb();
    
    char buffer[8];  // Store "#RRGGBB\n"
    uint8_t index = 0;
    
    while (1) {
        char c = uart_rx();  // Read one character
        
        if (c == '\n' || c == '\r') {  // End of line
            if (index == 7 && buffer[0] == '#') {
                set_rgb(
                    get_color_value(buffer[1], buffer[2]),  // Red
                    get_color_value(buffer[3], buffer[4]),  // Green
                    get_color_value(buffer[5], buffer[6])   // Blue
                );
                // Parse the color: buffer = "#RRGGBB"
                // Extract RR, GG, BB and call set_rgb()
            }
            index = 0;  // Reset for next color
        } else if (index < 7) {
            buffer[index++] = c;  // Store character
        }
    }
}
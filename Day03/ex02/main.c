#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_rgb();
void set_rgb(uint8_t r, uint8_t g, uint8_t b);

void wheel(uint8_t pos) 
{
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
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

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    // Just write to the Output Compare Registers!
    OCR0B = r;   // Red
    OCR0A = g;   // Green  
    OCR2B = b;   // Blue
}

int main()
{
        init_rgb();
    
    uint8_t pos = 0;
    while (1) {
        wheel(pos);
        pos++;
        _delay_ms(10);  // Smooth transition speed
    }
}
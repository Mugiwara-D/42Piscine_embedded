#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL  // Define CPU frequency as 16 MHz if not already defined
#endif

// Button pins (adjust based on your board!)
#define SW1_PIN PD2  // Increment button
#define SW2_PIN PD4 // Decrement button

uint8_t button_pressed(uint8_t pin) {
    // Check if button is pressed (LOW)
    if (!(PIND & (1 << pin))) {
        _delay_ms(20);  // Wait for bounce to settle
        
        // Check again - still pressed?
        if (!(PIND & (1 << pin))) {
            // Wait for button release
            while (!(PIND & (1 << pin)))
                ;  // Stay here until released
            
            _delay_ms(20);  // Debounce the release too
            return 1;  // Valid press detected
        }
    }
    return 0;  // No press or false trigger
}

int main(void) {
    // Step 1: Configure LED pin as OUTPUT
    DDRB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
    
    // Step 2: Configure button pins as INPUT with pull-ups
    DDRD &= ~((1 << SW1_PIN) | (1 << SW2_PIN));  // Set as INPUT
    PORTD |= (1 << SW1_PIN) | (1 << SW2_PIN);    // Enable pull-ups
    
    // Step 3: Configure Timer1 for PWM (same as Ex02)
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);
    ICR1 = 15624;
    
    // Step 4: Initialize duty cycle to 10%
    uint8_t duty_cycle = 10;
    OCR1A = (ICR1 * duty_cycle) / 100;
    
    // Step 5: Main loop - check buttons and update duty cycle
    while (1) 
    {
        // Check SW1 (increment)
        if (button_pressed(SW1_PIN)) 
        {
            if (duty_cycle < 100) 
            {
                duty_cycle += 10;
                OCR1A = ((uint32_t)ICR1 * duty_cycle) / 100;
            }
        }
        
        // Check SW2 (decrement)
        if (button_pressed(SW2_PIN)) 
        {
            if (duty_cycle > 10) 
            {
                duty_cycle -= 10;
                OCR1A = ((uint32_t)ICR1 * duty_cycle) / 100;
            }
        }
        if (duty_cycle == 100) 
            PORTB |= (1 << PB2);  // Turn on indicator LED
        else
            PORTB &= ~(1 << PB2); // Turn off indicator LED
        if (duty_cycle == 10) 
            PORTB |= (1 << PB0);  // Turn on indicator LED
        else
            PORTB &= ~(1 << PB0); // Turn off indicator LED


    }
}
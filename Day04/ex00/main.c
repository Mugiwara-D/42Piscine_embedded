#include <avr/io.h>
#include <avr/interrupt.h>

// External interrupt: button pressed
ISR(INT0_vect)
{
    PORTB ^= (1 << PORTB0);        // Toggle LED
    
    EIMSK &= ~(1 << INT0);          // Disable INT0 temporarily
    
    // Start Timer0 for debouncing
    TCNT0 = 0;                      // Reset counter
    TCCR0B |= (1 << CS02) | (1 << CS00);  // Start timer with prescaler 1024
}

// Timer interrupt: debounce period expired
ISR(TIMER0_COMPA_vect)
{
    TCCR0B = 0;                     // Stop timer
    EIMSK |= (1 << INT0);           // Re-enable INT0
}

int main(void) 
{   
    // LED setup
    DDRB |= (1 << DDB0);
    
    // Button setup (with pull-up)
    DDRD &= ~(1 << DDD2);
    PORTD |= (1 << PORTD2);
    
    // External interrupt setup (INT0, falling edge)
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    
    // Timer0 setup (CTC mode, not started yet)
    TCCR0A |= (1 << WGM01);         // CTC mode
    OCR0A = 250;                    // Compare value for ~16ms
    TIMSK0 |= (1 << OCIE0A);        // Enable compare match interrupt
    
    sei();      
    
    while (1) {}
}
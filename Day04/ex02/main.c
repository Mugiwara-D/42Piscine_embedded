#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t counter = 0;
volatile uint8_t debounce_int0 = 0;
volatile uint8_t debounce_pcint = 0;

void update_leds(uint8_t value) {
    PORTB &= ~0b00010111; // Clear relevant bits
    PORTB |= (value & 0b0111);// Update lower 3 bits
    PORTB |= ((value & 0b1000) << 1);// Update 4th bit to PB4
}

ISR(INT0_vect) {
    if (!debounce_int0) {
        counter = (counter + 1) & 0x0F;
        update_leds(counter);
        
        debounce_int0 = 1;
        EIMSK &= ~(1 << INT0);
        TCNT1 = 0;
        TCCR1B = (1 << WGM12) | (1 << CS12);  // Start Timer1, prescaler 256
    }
}

ISR(PCINT2_vect) {
    if (!(PIND & (1 << PIND4)) && !debounce_pcint) {
        counter = (counter - 1) & 0x0F;
        update_leds(counter);
        
        debounce_pcint = 1;
        PCMSK2 &= ~(1 << PCINT20);
        TCNT1 = 0;
        TCCR1B = (1 << WGM12) | (1 << CS12);  // Start Timer1
    }
}

ISR(TIMER1_COMPA_vect) {
    TCCR1B = (1 << WGM12);  // Stop timer (keep CTC mode, clear prescaler)
    
    if (debounce_int0) {
        debounce_int0 = 0;
        EIMSK |= (1 << INT0);
    }
    
    if (debounce_pcint) {
        debounce_pcint = 0;
        PCMSK2 |= (1 << PCINT20);
    }
}

int main(void) {
    // LED setup
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
    
    // Button setup
    DDRD &= ~((1 << DDD2) | (1 << DDD4));
    PORTD |= (1 << PORTD2) | (1 << PORTD4);
    
    // INT0 setup
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    
    // PCINT20 setup
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20);
    
    // Timer1 setup for 50ms debounce
    TCCR1B = (1 << WGM12);   // CTC mode, timer stopped initially
    OCR1A = 3124;             // 50ms @ 256 prescaler
    TIMSK1 = (1 << OCIE1A);  // Enable compare match interrupt
    
    update_leds(0);
    sei();
    
    while (1) {}
}
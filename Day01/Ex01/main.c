#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL  // Define CPU frequency as 16 MHz if not already defined
#endif



int main(void) 
{  
    // Configure PB1 as OUTPUT to allow Timer1 to control LED D2
    // DDRB: Data Direction Register for Port B
    // Setting bit 1 = OUTPUT mode (Datasheet Section 14.2.1)
    DDRB |= (1 << PB1);
    
    // Initialize Timer1 counter to 0 (optional, but good practice)
    TCNT1 = 0;
    
    // Configure Timer1 Control Register A
    // COM1A0 = 1: Toggle OC1A (PB1) on Compare Match (Table 16-3)
    // COM1A1 = 0: (combined with COM1A0=1 gives toggle mode)
    TCCR1A = (1 << COM1A0);
    
    // Configure Timer1 Control Register B
    // WGM12 = 1: CTC mode (Clear Timer on Compare Match) (Table 16-4)
    // CS12 = 1, CS11 = 0, CS10 = 0: Prescaler = 256 (Table 16-5)
    TCCR1B = (1 << WGM12) | (1 << CS12);
    
    // Set Output Compare Register for 1 Hz LED blink frequency
    // Timer frequency after prescaler: 16 MHz / 256 = 62,500 Hz
    // For toggle every 0.5s (to achieve 1 Hz blink):
    // OCR1A = (F_CPU / (Prescaler × 2)) - 1
    //       = (16,000,000 / (256 × 2)) - 1
    //       = 31,249
    // This makes the pin toggle every 0.5s → 1 Hz frequency
    OCR1A = 31249;

    // Infinite loop - Timer1 handles LED toggling automatically
    while (1) {
    }
}
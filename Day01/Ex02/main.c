#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL  // Define CPU frequency as 16 MHz if not already defined
#endif



int main(void) 
{  
    // Step 1: Configure PB1 as OUTPUT
    DDRB |= (1 << 1);
    
    // Step 2: Configure TCCR1A for:
    //   - Non-inverting PWM on OC1A (COM1A1=1, COM1A0=0)
    //   - Fast PWM mode, part 1 (WGM11=1, WGM10=0)
    // (Datasheet Table 16-3: Compare Output Mode, Non-PWM)
    // (Datasheet Table 16-4: Waveform Generation Mode Bit Description)
    TCCR1A = (1 << 7) | (1 << 1);
    
    // Step 3: Configure TCCR1B for:
    //   - Fast PWM mode, part 2 (WGM13=1, WGM12=1)
    //   - Prescaler 1024 (CS12=1, CS10=1)
    // (Datasheet Table 16-4: Waveform Generation Mode)
    // (Datasheet Table 16-5: Clock Select Bit Description)
    TCCR1B = (1 << 4) | (1 << 3) | (1 << 2) | (1 << 0);
    
    // Step 4: Set TOP value (period = 1 second)
    //   ICR1 = (F_CPU / Prescaler / Frequency) - 1
    //        = (16,000,000 / 1024 / 1) - 1
    //        = 15,624
    // (Datasheet Section 16.11.7: Input Capture Register)
    ICR1 = 15624;
    
    // Step 5: Set compare value (10% duty cycle)
    //   OCR1A = ICR1 × 0.10
    //         = 15,624 × 0.10
    //         = 1,562
    // (Datasheet Section 16.11.6: Output Compare Register)
    OCR1A = 1562;
    while (1) 
    {}
}
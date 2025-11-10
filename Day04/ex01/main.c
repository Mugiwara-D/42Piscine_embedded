#include <avr/io.h>
#include <avr/interrupt.h>

volatile int16_t duty_cycle = 0;
volatile int8_t direction = 1;
volatile uint8_t step = 10;

ISR(TIMER0_COMPA_vect)
{
    duty_cycle += (step * direction);
    
    if (duty_cycle >= 1999) {
        duty_cycle = 1999;
        direction = -1;
    }
    else if (duty_cycle <= 0) {
        duty_cycle = 0;
        direction = 1;
    }
    
    OCR1A = duty_cycle;
}

int main(void) 
{   
    DDRB |= (1 << DDB1);

    // Timer1: Fast PWM, Mode 14, Non-inverted, Prescaler 8
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 1999;
    OCR1A = 0;
    
    // Timer0: CTC mode, Prescaler 256, ~2.5ms period
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS02);
    OCR0A = 156;
    TIMSK0 |= (1 << OCIE0A);
    
    sei();
    
    while (1) {}
}
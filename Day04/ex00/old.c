#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect)
{
    PORTB ^= (1 << PORTB0); 
}

int main(void) 
{   
    DDRB |= (1 << DDB0);       //set PB0 as output

    DDRD &= ~(1 << DDD2);    // Set PD2 as input (optional, it's input by default)
    PORTD |= (1 << PORTD2); // Enable internal pull-up resistor
    
    EICRA |= (1 << ISC01); // This sets ISC01=1, ISC00=0 → Falling edge
    EIMSK |= (1 << INT0); // Enable INT0
    
    sei();      
    while (1)
    {}
}
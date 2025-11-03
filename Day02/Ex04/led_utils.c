#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "utils.h"

void led_init(void)
{
    // Set PB5 as output
    DDRB |= (1 << DDB4) | (1 << DDB0) | (1 << DDB1) | (1 << DDB2);
}

void led_success_effect(void)
{
    for (int i = 0; i < 3; i++)
    {
        // Turn LED on
        PORTB |= (1 << PORTB4);
        _delay_ms(200);
        PORTB &= ~(1 << PORTB4);
        PORTB |= (1 << PORTB2);
        _delay_ms(200);
        PORTB &= ~(1 << PORTB2);
        PORTB |= (1 << PORTB1);
        _delay_ms(200);
        PORTB &= ~(1 << PORTB1);
        PORTB |= (1 << PORTB0);
        _delay_ms(200);
        PORTB &= ~(1 << PORTB0);
        _delay_ms(200);
    }
}
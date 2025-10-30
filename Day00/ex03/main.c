#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB |= (1 << 0); // Set PB0 as output

    PORTD |= (1 << 2); // enable pull-up resistor on PD0
    
    uint8_t prev_state = 1; // previous state of the button assuming is not pressed
    
    while (1)
    {   
        uint8_t current_state = (PIND & (1 << 2)) >> 2; // read current state of the button
        

        if (prev_state == 1 && current_state == 0) // button press detected realesed to pressed
        {
            PORTB ^= (1 << 0); // toggle PB0 (LED state)
            _delay_ms(20); // simple debounce delay
        }
        prev_state = current_state; // update previous state
    }
    return 0;
}
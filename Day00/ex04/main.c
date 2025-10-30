#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 4); // Set LED1,2,3,4 as output can also be 0x17 

    PORTD |= (1 << 2) | (1 << 4); // enable pull-up resistor on PD0 sw1 is connected to PD2 and sw2 is connected to PD4 can be 0x14
    
    uint8_t counter = 0;
    uint8_t prev_sw1 = 1; // previous state of the button assuming is not pressed
    uint8_t prev_sw2 = 1; 
    
    while (1)
    {   
        uint8_t current_sw1 = (PIND & (1 << 2)) >> 2; // read current state of the sw1 button
        uint8_t current_sw2 = (PIND & (1 << 4)) >> 4; // read current state of the sw2 button
        

        if (prev_sw1 == 1 && current_sw1 == 0) 
        {
            counter++;
            if (counter > 15)
                counter = 0;
            _delay_ms(50); // simple debounce delay
        }
        if (prev_sw2 == 1 && current_sw2 == 0) 
        {
            if (counter == 0) // to handle underflow
                counter = 15;
            else
                counter--;
            _delay_ms(50); // simple debounce delay
        }
        
        PORTB &= ~0x17; // Clear LED1,2,3,4
        PORTB |= (counter & 0x07); // Set LED1,2,3 according to counter value
        
        if (counter & 0x08) // check if 4th bit is set
            PORTB |= (1 << 4); // turn on LED4 
         
        prev_sw1 = current_sw1; // update previous state
        prev_sw2 = current_sw2; 
    }
    return 0;
}
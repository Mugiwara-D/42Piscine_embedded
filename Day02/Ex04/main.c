#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "utils.h"

// Input buffer and state tracking
char input_buffer[MAX_INPUT];
uint8_t buffer_index = 0;
login_state_t current_state = STATE_USERNAME;

ISR(USART_RX_vect)
{
    char received = UDR0;
    
    // Handle special keys
    if (received == '\r' || received == '\n') 
        process_enter();
    else if (received == '\b' || received == 0x7F)   // Backspace or DEL
        process_backspace();
    else if (buffer_index < MAX_INPUT - 1) {
        // Add to buffer
        input_buffer[buffer_index++] = received;
        
        // Echo character
        if (current_state == STATE_USERNAME)
            uart_tx(received);  
        else if (current_state == STATE_PASSWORD)
            uart_tx('*');       
    }
}

int main(void)
{
    uart_init(8);
    led_init();
    sei();
    
    uart_printstr("\r\nEnter your login:\r\n");
    uart_printstr("username: ");
    
    while (1) {}
    
    return 0;
}
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "utils.h"

char saved_username[MAX_INPUT];
char saved_password[MAX_INPUT];

char    *ft_strcpy(char *dest, const char *src)
{
    char *original_dest = dest;
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void ft_memset(void *s, int c, int n)
{
    unsigned char *ptr = s;
    while (n--)
        *ptr++ = (unsigned char)c;
}

void clear_buffer(void)
{
    ft_memset(input_buffer, 0, MAX_INPUT);
    buffer_index = 0;
}

void process_enter(void)
{
    if (current_state == STATE_USERNAME)
    {
        ft_strcpy(saved_username, input_buffer);  
        clear_buffer();
        
        uart_printstr("\r\npassword: ");
        current_state = STATE_PASSWORD;
    }
    else if (current_state == STATE_PASSWORD)
    {
        // Save the password
        ft_strcpy(saved_password, input_buffer);
        uart_printstr("\r\n");
        
        // Check BOTH credentials
        if (ft_strcmp(saved_username, USERNAME) == 0 && 
            ft_strcmp(saved_password, PASSWORD) == 0)
        {
            uart_printstr("Hello ");
            uart_printstr(saved_username);
            uart_printstr("!\r\n");
            _delay_ms(1000);  
            uart_printstr("Shall we play a game?\r\n");
            
            current_state = STATE_LOGGED_IN;
            led_success_effect();
        }
        else
        {
            uart_printstr("Bad combination username/password\r\n\r\n");
            uart_printstr("Enter your login:\r\n");
            uart_printstr("username: ");
            current_state = STATE_USERNAME;
        }
        clear_buffer();
    }
}

void process_backspace(void)
{
    if (buffer_index > 0)
    {
        buffer_index--;
        input_buffer[buffer_index] = '\0';
        // Move cursor back, overwrite with space, move cursor back again
        uart_printstr("\b \b");
    }
}
#ifndef UTILS_H
#define UTILS_H

#define USERNAME "spectre"
#define PASSWORD "snoopy"
#define MAX_INPUT 32

// Input buffer and state tracking
//char input_buffer[MAX_INPUT];
//uint8_t buffer_index = 0;

// Login state
typedef enum {
    STATE_USERNAME,
    STATE_PASSWORD,
    STATE_LOGGED_IN
} login_state_t;

// Global variable declarations
extern login_state_t current_state;
extern char input_buffer[MAX_INPUT];
extern uint8_t buffer_index;

// Function prototypes
void uart_init(unsigned int ubrr);
void uart_tx(char c);
void uart_printstr(const char* str);
void clear_buffer(void);
void process_enter(void);
void process_backspace(void);
void led_init(void);
void led_success_effect(void);
void ft_memset(void *s, int c, int n);
int ft_strcmp(const char *s1, const char *s2);

#endif // UTILS_H
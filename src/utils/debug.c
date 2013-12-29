/**
 * @file debug.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#include "debug.h"

// Do we want debug?
#ifdef UTILS_DEBUG

void debug_init(void) {
    // Get default usart config
    usart_config_t config;
    usart_get_default_config(&config);
    
    // Enable double speed
    config.doublespeed = 1;
    // Disable receive
    config.enable_rx = 0;
    
    // Initialize usart with config
    usart_init(&config);
    
    // Send debug message
    debug_string_p(PSTR("\r\nUSART debug started\r\n"));
}

void debug_string(char *string) {
    usart_send_string(string);
}

void debug_string_p(const char *pstring) {
    usart_send_string_p(pstring);
}

void debug_number_(uint16_t value, uint8_t base) {
    // Create buffer
    char buffer[8*sizeof(uint16_t)+1];
    char *str = &buffer[sizeof(buffer)-1];
    // Set ending \0
    *str = '\0';
    uint16_t tmp;
    uint8_t c;
    
    // Make sure we have a base larger than 1
    if (base < 2)
        base = 10;
    
    // Iterate over value
    do {
        // Set temp value
        tmp = value;
        // Calc divider
        value /= base;
        // Get digit
        c = tmp - base * value;
        *--str = c < 10 ? '0' + c : 'A' + c - 10;
    } while (value);
    
    // Send string
    usart_send_string(str);
}

void debug_number(uint16_t value) {
    debug_number_(value, 10);
}

void debug_number_as_hex(uint16_t value) {
    debug_number_(value, 16);
}

void debug_array(uint8_t *data, uint16_t length, char glue) {
    while (length--) {
        usart_send(*data++);
        usart_send(glue);
    }
}

#endif // UTILS_DEBUG

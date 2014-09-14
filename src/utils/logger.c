/**
 * @file logger.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#include "logger.h"

// Do we want logging?
#if defined(UTILS_LOGGER_INFO) || defined(UTILS_LOGGER_DEBUG)

void logger_init(void) {
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
    info_string_p(PSTR("\r\nUSART logger started\r\n"));
    info_string_p(PSTR("ethshield version: "));
    info_string_p(PSTR(VERSION));
    info_string_p(logger_newline);
}

void logger_string(char *string) {
    usart_send_string(string);
}

void logger_string_n(char *string, uint16_t length) {
    usart_send_string_n(string, length);
}

void logger_string_p(const char *pstring) {
    usart_send_string_p(pstring);
}

void logger_number_(uint16_t value, uint8_t base) {
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

void logger_number(uint16_t value) {
    logger_number_(value, 10);
}

void logger_number_as_hex(uint16_t value) {
    logger_number_(value, 16);
}

void logger_array(uint8_t *data, uint16_t length, char glue) {
    while (length--) {
        usart_send(*data++);
        usart_send(glue);
    }
}

void logger_ip(uint8_t *addr) {
    logger_number(addr[0]);
    usart_send(0x2E); // .
    logger_number(addr[1]);
    usart_send(0x2E); // .
    logger_number(addr[2]);
    usart_send(0x2E); // .
    logger_number(addr[3]);
}

void logger_mac(uint8_t *addr) {
    logger_number_as_hex(addr[0]);
    usart_send(0x3A); // :
    logger_number_as_hex(addr[1]);
    usart_send(0x3A); // :
    logger_number_as_hex(addr[2]);
    usart_send(0x3A); // :
    logger_number_as_hex(addr[3]);
    usart_send(0x3A); // :
    logger_number_as_hex(addr[4]);
    usart_send(0x3A); // :
    logger_number_as_hex(addr[5]);
}

const char logger_newline[] PROGMEM = "\r\n";
const char logger_dot[] PROGMEM = ".";
const char logger_ok[] PROGMEM = " [ok]\r\n";
const char logger_error[] PROGMEM = " [error]\r\n";

#endif // UTILS_LOGGER_INFO || UTILS_LOGGER_DEBUG

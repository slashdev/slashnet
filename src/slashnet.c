/**
 * @file debug.h
 * @brief Initializer and main loop of /Net
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

// Speed of micro-controller (20.0 MHz)
#ifndef F_CPU
#define F_CPU 20000000
#endif

#include <inttypes.h>
#include <avr/interrupt.h>
#include "ext/www_server.h"
#include "net/network.h"
#include "utils/logger.h"
#include "utils/uptime.h"
#include "utils/werkti.h"

void www_root(uint8_t type, uint8_t *data) {
    www_server_reply_header(HTTP_STATUS_200, HTTP_CONTENT_TYPE_PLAIN);
    www_server_reply_add_p(PSTR("Version: "));
    www_server_reply_add_p(PSTR(VERSION));
    www_server_reply_send();
}

const char status[] PROGMEM = "Status!";

void www_status(uint8_t type, uint8_t *data) {
    www_server_reply_header(HTTP_STATUS_200, HTTP_CONTENT_TYPE_PLAIN);
    www_server_reply_add_p(PSTR("Running"));
    www_server_reply_send();
}

int main(void) {
    // Enable interrupts
    sei();

    // Initialize debug
    logger_init();

    // Initialize uptime
    uptime_init();

    // Initialize network chip
    network_init();

    // Initialize www server
    www_server_init();
    www_server_register_path(PSTR("/"), www_root);
    www_server_register_path(PSTR("/status"), www_status);

    // Infinite loop
    while (1) {
        // Handle network traffic
        network_backbone();
        // Maybe send werkti report
        werkti_maybe_report();
    }

}

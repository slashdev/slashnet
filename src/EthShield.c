/**
 * @file debug.h
 * @brief Initializer and main loop of EthShield
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
#include "net/network.h"
#include "utils/logger.h"
#include "utils/uptime.h"
#include "utils/werkti.h"

int main(void) {
	// Enable interrupts
    sei();
	
    // Initialize debug
    logger_init();
    
    // Initialize uptime
    uptime_init();
    
    // Initialize network chip
    network_init();
    
    // Infinite loop
    while (1) {
        // Handle network traffic
        network_backbone();
        // Maybe send werkti report
        werkti_maybe_report();
    }
    
}

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
#include "utils/debug.h"

int main(void) {
	// Enable interrupts
    sei();
	
    // Initialize debug
    debug_init();
    
    // Initialize network chip
    network_init();
    
    // Infinite loop
    while (1) {
        
    }
    
}

/**
 * Copyright (C) Ferdi van der Werf. All rights reserved
 * 
 * Version: 0.1.0
 * This file is part of EthShield, distributed under the MIT license.
 */

// Speed of micro-controller (20.0 MHz)
#ifndef F_CPU
#define F_CPU 20000000
#endif

#include <inttypes.h>
#include "utils/debug.h"

int main(void) {
	// Enable interrupts
    sei();
	
    debug_init();
    
    // Infinite loop
    while (1) {
        
    }
    
}

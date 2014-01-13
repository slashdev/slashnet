/**
 * @file i2c.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.11.0
 */

#include "i2c.h"

// Do we want I2C?
#ifdef COM_I2C

uint8_t flags;

#define I2C_FLAG_INITIALIZED
#define I2C_FLAG_STARTED

void i2c_init(void) {
    // Check if we already initialized I2C
    if (initialized) {
        return;
    }
    
    // Set I2C as initialized
    initialized = 1;
}

#endif // COM_I2C

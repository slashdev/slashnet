/**
 * @file tlc59116.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.12.0
 */

#include "tlc59116.h"

// Do we want TLC59116?
#ifdef EXT_TLC59116

// Check if COM_I2C is enabled
#ifndef COM_I2C
#error TLC59116 cannot work without COM_I2C
#endif // COM_I2C

void tlc59116_reset(void) {
    i2c_init(350); // Make sure i2c is initialized
    // Start i2c
    if (i2c_start()) { /* Start failed */ }
    // Send reset command
    else if(i2c_send_address(0xD6)) { /* Send reset command failed */ }
    // Send first reset byte
    else if(i2c_send_byte(0xA5)) { /* Send first reset byte failed */ }
    // Send second reset byte
    else if(i2c_send_byte(0x5A)) { /* Send second reset byte failed */ }
    // Stop i2c
    i2c_stop();
}

#endif // EXT_TLC59116

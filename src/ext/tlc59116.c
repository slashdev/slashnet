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

#endif // EXT_TLC59116

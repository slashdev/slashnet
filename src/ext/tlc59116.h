/**
 * @file tlc59116.h
 * @brief Led driver TLC59116 functionality
 *
 * Basic functions for communicating with led driver TLC59116.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.12.0
 */

#ifndef EXT_TLC59116_H
#define EXT_TLC59116_H

#include "../config.h"

// Do we want TLC59116
#ifdef EXT_TLC59116

#include <inttypes.h>
#include "../com/i2c.h"

/**
 * @brief Soft reset all connected chips
 */
void tlc59116_reset(void);

#endif // EXT_TLC59116
#endif // EXT_TLC59116_H

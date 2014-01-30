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
#include <util/delay.h>
#include "../com/i2c.h"

/**
 * @brief Soft reset all connected chips
 */
void tlc59116_reset(void);

/**
 * @brief Wake crystal on chip with address
 * @note After sending i2c command it waits 500us for chip (see documentation
 * page 13, table 3, footnote 3).
 * @note Assumes I2C works without error
 * @param address Address of chip to wake up
 * @param sleep 0 to wake up, >0 to sleep
 */
void tlc59116_sleep(uint8_t address, uint8_t sleep);

#endif // EXT_TLC59116
#endif // EXT_TLC59116_H

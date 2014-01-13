/**
 * @file i2c.h
 * @brief I2C (TWI) functionality
 *
 * Basic functions for communicating with I2C slave devices. This implementation
 * is limited to a single master on a I2C bus.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.2.0
 */

#ifndef COM_I2C_H
#define COM_I2C_H

#include "../config.h"

// Do we want I2C?
#ifdef COM_I2C

#include <inttypes.h>
#include <avr/io.h>

/**
 * Initialize the I2C master. Only needs to be called once
 */
void i2c_init(uint16_t bitrateKHz);

/**
 * @brief Issue start condition, send address and transfer direction
 * @param address Address and transfer direction of slave device
 * @retval 0 Device accessible
 * @retval 1 Failed to access device
 */
uint8_t i2c_start(uint8_t address);

/**
 * Terminate the data transfer and release the I2C bus
 */
void i2c_stop(void);


#endif // COM_I2C
#endif // COM_I2C_H

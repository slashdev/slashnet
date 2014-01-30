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
#include <util/delay.h>
#include <util/twi.h>
#include "../utils/logger.h"

/**
 * @brief Initialize the I2C interface
 * @param bitrateKhz Bitrate in Khz for I2C to use
 */
extern void i2c_init(uint16_t bitrateKHz);

/**
 * @brief Disable the I2C interface
 */
extern void i2c_disable(void);

/**
 * @brief Issue start condition
 * @retval 0 Start or repeated start success
 * @retval 1 Timeout
 * @retval >1 Failed, status register
 */
extern uint8_t i2c_start(void);

/**
 * @brief Issue stop condition
 * @retval 0 Stop success
 * @retval 1 Timeout
 */
extern uint8_t i2c_stop(void);

/**
 * @brief Send address and transfer direction
 * @param address Address and transfer direction of slave device
 * @retval 0 Device accessible
 * @retval 1 Timeout
 * @retval >1 Failed, status register
 */
extern uint8_t i2c_send_address(uint8_t address);

/**
 * @brief Send byte over i2c
 * @param byte Byte to send
 * @retval 0 Byte send and ack'ed
 * @retval 1 Timeout
 * @retval >1 Failed, status register
 */
extern uint8_t i2c_send_byte(uint8_t byte);


#endif // COM_I2C
#endif // COM_I2C_H

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
 * @since 0.11.0
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
 * @brief Set address with write bit
 */
#define I2C_WRITE(x) ((x) & ~0x01)
/**
 * @brief Set address with read bit
 */
#define I2C_READ(x) ((x) | 0x01)

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

/**
 * @brief Set register over i2c
 * @param address Address of chip
 * @param reg Register to write to
 * @param value Value to write to register
 * @retval 0 Register set
 * @retval 1 Failed
 */
extern uint8_t i2c_write_register(uint8_t address, uint8_t reg, uint8_t byte);

/**
 * @brief Receive a byte over i2c
 * @param ack True if we need to send an ack back, false otherwise
 * @param *data Byte by ref to put received byte in
 * @retval 0 Byte received
 * @retval 1 Timeout
 * @retval >1 Failed, status register
 */
extern uint8_t i2c_receive_byte(uint8_t ack, uint8_t *data);


#endif // COM_I2C
#endif // COM_I2C_H

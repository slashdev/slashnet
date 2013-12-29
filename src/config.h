/**
 * @file config.h
 * @brief Configuration of EthShield
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

/**
 * Clock setting
 */
#ifndef F_CPU
#define F_CPU 20000000
#endif

/**
 * Communication channels
 */

// SPI
// Enable SPI?
#define COM_SPI

// UART
// Enable UART?
#define COM_UART
// Define buffer ring size
#define COM_UART_BUFFER_RING_SIZE 64

/**
 * Utilities
 */

// Debug
#define UTILS_DEBUG

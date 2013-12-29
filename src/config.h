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
/**
 * @brief Enables Serial Peripheral Interface channel
 */
#define COM_SPI

// UART
/**
 * @brief Enable Universal Synchronous and Asymchronous serial Receiver and 
 * Transmitter channel
 */
#define COM_USART
/**
 * @brief USART buffer ring size
 */
#define COM_USART_BUFFER_RING_SIZE 64

/**
 * Utilities
 */

// Debug
/**
 * @brief Enable debug over USART
 */
#define UTILS_DEBUG

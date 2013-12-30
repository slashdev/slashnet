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
 * @brief Version of the EthShield firmware
 */
#define VERSION "0.2.0"

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
 * Network
 */

/**
 * @brief Enable network
 */
#define NET_NETWORK
/**
 * @brief Network buffer in size
 */
#define BUFFER_IN_SIZE 1023
/**
 * @brief Network buffer out size
 */
#define BUFFER_OUT_SIZE 1023

/**
 * Utilities
 */

// Debug
/**
 * @brief Enable debug over USART
 */
#define UTILS_DEBUG


/**********************************************************************
 * DO NOT CHANGE BELOW
 * - References from config.c, change them in config.c
 * - Logic to check for bad config
 ***********************************************************************/

#if BUFFER_IN_SIZE > 1500
#error BUFFER_IN_SIZE larger than network chip can handle
#endif
#if BUFFER_OUT_SIZE > 1500
#error BUFFER_OUT_SIZE larger than network chip can handle
#endif

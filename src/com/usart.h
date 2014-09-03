/**
 * @file usart.h
 * @brief Universal Synchronous and Asymchronous serial Receiver and Transmitter
 * channel functionality
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#ifndef COM_USART_H
#define COM_USART_H

#include "../config.h"

// Do we want UART?
#ifdef COM_USART

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/**
 * USART config file
 */
typedef struct {
    /**
     * @brief Requested baudrate to communicate on
     */
    uint16_t baudrate;
    /**
     * @brief Double the transmission speed
     * @note Only effective in asynchronous mode, in synchronous mode it is set to 0
     */
    uint8_t doublespeed;
    /**
     * @brief Multi-processor mode
     * When enabled, all incoming frames without address information are ignored.
     */
    uint8_t multiprocessormode;
    /**
     * @brief Enable receive
     */
    uint8_t enable_rx;
    /**
     * @brief Enable transmission
     */
    uint8_t enable_tx;
} usart_config_t;

/**
 * @brief Initialize USART channel
 * @param config Configuration for usart channel
 */
extern void usart_init(usart_config_t *config);

/**
 * @brief Set default config for USART
 * Default config is:
 * - Baudrate: 9600
 * - Double speed: no
 * - Multi-processor mode: no
 * - Enable receive: yes
 * - Enable transmit: yes
 */
extern void usart_get_default_config(usart_config_t *config);

/**
 * @brief Send a byte
 * @param data Byte to send
 */
extern void usart_send(uint8_t data);
/**
 * @brief Send an array (or string)
 * @param data Array (or string) to send
 */
extern void usart_send_string(char *data);
/**
 * @brief Send an array (or string) with length
 * @param data Array (or string) to send
 * @param length Length of array/string to send
 */
extern void usart_send_string_n(char *data, uint16_t length);
/**
 * @brief Send an array (or string) from progmem
 * @param pdata Array (or string) to send
 */
extern void usart_send_string_p(const char *data);

#endif // COM_USART
#endif // COM_USART_H

/*!
 * Copyright (C) Ferdi van der Werf. All rights reserved
 *
 * This file is part of EthShield, distributed under the MIT license.
 */

#ifndef COM_UART_H
#define COM_UART_H

#include "../config.h"

// Do we want UART?
#ifdef COM_UART

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/**
 * UART config file
 */
typedef struct uart_config {
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
} uart_config_t;

/**
 * @brief Initialize UART channel
 * @param baudrate Baudrate you want to use
 * @param doublespeed 1 for double speed, 0 for normal speed
 */
void uart_init(uint16_t baudrate, uint8_t doublespeed);

/**
 * @brief Set default config for UART
 * Default config is:
 * - Baudrate: 9600
 * - Double speed: no
 * - Multi-processor mode: no
 * - Enable receive: yes
 * - Enable transmit: yes
 */
void uart_set_default_config(uart_config_t *config);

/**
 * @brief Send a byte
 * @param data Byte to send
 */
void uart_send(uint8_t data);
/**
 * @brief Send an array (or string)
 * @param data Array (or string) to send
 */
void uart_send_array(uint8_t *data);
/**
 * @brief Send an array (or string) from progmem
 * @param pdata Array (or string) to send
 */
void uart_send_array_p(const uint8_t *data);

#endif // COM_UART
#endif // COM_UART_H
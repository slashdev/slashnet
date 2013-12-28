/**
 * Copyright (C) Ferdi van der Werf. All rights reserved
 *
 * This file is part of EthShield, distributed under the MIT license.
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

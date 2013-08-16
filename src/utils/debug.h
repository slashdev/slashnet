/*
 * Copyright (C) Ferdi van der Werf. All rights reserved
 *
 * This file is part of EthShield, distributed under the MIT license.
 */

#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include "../config.h"

// Do we want debug?
#ifdef UTILS_DEBUG

// Make sure UART is enabled
#ifndef COM_UART
#warning Using UTILS_DEBUG while COM_UART not enabled, forcing COM_UART
#define COM_UART
#endif // COM_UART

#include <inttypes.h>
#include <util/delay.h>
#include "../com/uart.h"

extern void debug_init(void);
extern void debug_string(char *string);
extern void debug_string_p(const char *pstring);
extern void debug_number(uint16_t value);
extern void debug_number_as_hex(uint16_t value);
extern void debug_array(uint8_t *data, char glue);

#else // UTILS_DEBUG

// No debug wanted, create placeholders
#define debug_init(...) do {} while (0)
#define debug_string(...) do {} while (0)
#define debug_string_p(...) do {} while (0)
#define debug_number(...) do {} while (0)
#define debug_number_as_hex(...) do {} while (0)
#define debug_array(...) do {} while (0)

#endif // UTILS_DEBUG
#endif // UTILS_DEBUG_H

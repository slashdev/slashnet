/**
 * @file debug.h
 * @brief Debug over uart functionality
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include "../config.h"

// Do we want debug?
#ifdef UTILS_DEBUG

// Make sure USART is enabled
#ifndef COM_USART
#warning Using UTILS_DEBUG while COM_USART not enabled, forcing COM_USART
#define COM_USART
#endif // COM_USART

#include <inttypes.h>
#include <util/delay.h>
#include "../com/usart.h"

extern void debug_init(void);
extern void debug_string(char *string);
extern void debug_string_p(const char *pstring);
extern void debug_number(uint16_t value);
extern void debug_number_as_hex(uint16_t value);
extern void debug_array(uint8_t *data, uint16_t length, char glue);

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

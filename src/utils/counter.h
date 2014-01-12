/**
 * @file counter.h
 * @brief Time counter usable for DHCP, tcp, uptime or other time management
 *
 * This contains the functionality for second counting, all three timers on the
 * chip can be used as a second timer. If uptime is enabled it will use the
 * timer specified in the config.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.4.0
 */

#ifndef UTILS_COUNTER_H
#define UTILS_COUNTER_H

#include "../config.h"

// Do we want to use the counter?
#ifdef UTILS_COUNTER

#include <inttypes.h>
#include <avr/interrupt.h>
#include "uptime.h"
#include "werkti.h"
#include "../net/dhcp.h"

/**
 * @brief Initialize the selected timer for counting
 */
extern void counter_init(void);

/**
 * @brief Returns 1 if the counter is running
 */
extern uint8_t counter_is_running(void);

#endif // UTILS_COUNTER
#endif // UTILS_COUNTER_H

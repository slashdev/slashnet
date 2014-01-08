/**
 * @file uptime.h
 * @brief Uptime tracker
 *
 * This contains the functionality for uptime tracking and periodic calls.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.5.0
 */

#ifndef UTILS_UPTIME_H
#define UTILS_UPTIME_H

#include "../config.h"

// Do we want uptime?
#ifdef UTILS_UPTIME

#include <inttypes.h>

/**
 * Uptime struct
 */
typedef struct {
    uint16_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} uptime_t;


/**
 * @brief Initialize uptime data structure
 */
extern void uptime_init(void);

/**
 * @brief Update the uptime with a second
 */
extern void uptime_tick(void);

/**
 * @brief Uptime data structure
 */
extern uptime_t uptime;

#endif // UTILS_UPTIME
#endif // UTILS_UPTIME_H

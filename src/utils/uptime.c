/**
 * @file uptime.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.5.0
 */

#include "uptime.h"

// Do we want uptime?
#ifdef UTILS_UPTIME

uptime_t uptime;

void uptime_init(void) {
    uptime.seconds = 0;
    uptime.minutes = 0;
    uptime.hours = 0;
    uptime.days = 0;
}

void uptime_tick(void) {
    if (++uptime.seconds >= 60) {
        uptime.seconds = 0;
        if (++uptime.minutes >= 60) {
            uptime.minutes = 0;
            if (++uptime.hours >= 24) {
                uptime.hours = 0;
                uptime.days++;
            }
        }
    }
}

#endif // UTILS_UPTIME

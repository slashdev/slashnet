/**
 * @file network.h
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.3.0
 */

#include "network.h"

// Do we want network?
#ifdef NET_NETWORK

// Check if F_CPU is set
#ifndef F_CPU
#error F_CPU is not set but required for network
#endif // F_CPU

#endif // NET_NETWORK

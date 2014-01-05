/**
 * @file udp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf
 * @since
 */

#include "udp.h"

// Do we want UDP?
#ifdef NET_UDP

// Check if NET_NETWORK is enabled
#ifndef NET_NETWORK
#error UDP cannot work without NET_NETWORK
#endif // NET_NETWORK


#endif // NET_UDP

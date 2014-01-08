/**
 * @file dhcp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.9.0
 */

#include "dhcp.h"

// Do we want DHCP?
#ifdef NET_DHCP

// Check if UDP is enabled
#ifndef NET_UDP
#error DHCP cannot work without NET_UDP
#endif // NET_UDP



#endif // NET_DHCP

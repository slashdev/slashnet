/**
 * @file tcp.c
 *
 * \copyright Copyright 2014 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.14.0
 */


#include "tcp.h"

// Do we want TCP?
#ifdef NET_TCP

// Check if NET_NETWORK is enabled
#ifndef NET_NETWORK
#error TCP cannot work without NET_NETWORK
#endif // NET_NETWORK

// Port services list
#ifdef NET_TCP_SERVER
// Check if port list size is defined
#ifndef NET_TCP_SERVICES_LIST_SIZE
#error NET_TCP_SERVICES_LIST_SIZE not defined, but NET_TCP_SERVER active
#endif // NET_TCP_SERVICES_LIST_SIZE
// Create port service list
port_service_t port_services[NET_TCP_SERVICES_LIST_SIZE];
#endif // NET_TCP_SERVER


#ifdef NET_TCP_SERVER
#endif // NET_TCP_SERVER
#endif // NET_TCP

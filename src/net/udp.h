/**
 * @file udp.h
 * @brief User Datagram Protocol functionality
 *
 * This contains the prototype for all UDP functionality which is global
 * available. It allows you to register services to ports, prepare UDP packets
 * and send them. If you use the prototype functions like udp_prepare,
 * udp_prepare_reply and udp_send, all headers will be set correctly and
 * checksums will be calculated and added to the packet.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf
 * @since
 */

#ifndef NET_UDP_H
#define NET_UDP_H

#include "../config.h"

// Do we want UDP?
#ifdef NET_UDP



#endif // NET_UDP
#endif // NET_UDP_H

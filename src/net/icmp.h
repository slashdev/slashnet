/**
 * @file icmp.h
 * @brief Internet Control Message Protocol funcionality
 *
 * This contains functionality for ICMP packets. For example, you can be pinged
 * and a reply will be send back.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.8.0
 */

#ifndef NET_ICMP_H
#define NET_ICMP_H

#include "../config.h"

// Do we want ICMP?
#ifdef NET_ICMP

#include <inttypes.h>
#include "shared.h"
#include "network.h"
#include "../utils/werkti.h"

/**
 * @brief Handle received ICMP packets
 *
 * Looks if a received ICMP packet is a ping request, if so, send the reply.
 */
extern void icmp_packet_receive();

#endif // NET_ICMP
#endif // NET_ICMP_H

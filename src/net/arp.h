/**
 * @file arp.h
 * @brief Adres Resolution Protocol functionality
 *
 * This contains the prototype for all ARP functionality which is global
 * available. It allows others to request your MAC address and you to ask others
 * their MAC address by IP address.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.8.0
 */

#ifndef NET_ARP_H
#define NET_ARP_H

#include "../config.h"

// Do we want ARP?
#ifdef NET_ARP

#include <inttypes.h>
#include "network.h"
#include "shared.h"
#include "../utils/logger.h"
#include "../utils/werkti.h"

/**
 * @brief Initialize the ARP part.
 *
 * This includes preparing a cache for replies to arp requests.
 */
extern void arp_init(void);

/**
 * @brief ARP Packet loop handler.
 *
 * <i>network_backbone</i> calls this function when it verified an ARP packet is
 * in <i>buffer_in</i>. This function analyses that packet to see if it's a
 * request to respond to or a reply to a request which was send.
 */
extern void arp_receive(void);

/**
 * @brief Request the MAC address that corresponds to <i>ip_request</i>.
 *
 * @param ip_request The IP address to request the MAC address of
 */
extern uint8_t *arp_request_mac(uint8_t *ip_request);

#endif // NET_ARP
#endif // NET_ARP_H

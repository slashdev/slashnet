/**
 * @file dhcp.h
 * @brief Dynamic Host Configuration Protocol functionality
 *
 * The following RFCs are used in this implementation:
 * - DHCP Protocol: http://tools.ietf.org/html/rfc2131
 * - DHCP Options:  http://tools.ietf.org/html/rfc1533
 *
 * DHCP can be used in two ways.
 * - With renewal of lease
 * - Without renewal of lease
 *
 * By default, the network_init and network_backbone handle DHCP. Initial IP 
 * lookup is done in network_init and renewal is handlded by network_backbone. 
 * network_init should be called before your main loop and network_backbone 
 * inside your main loop.
 *
 * <h4>With renewal of lease</h4>
 * In config.h a counter needs to be enabled, which gets activated by 
 * counter_init(). <br />
 * Every second it updates the values of dhcp_seconds. <br />
 * To get the initial IP address you need to call dhcp_request_ip and feed it
 * the packets you receive, while it returns 0, no valid IP address could be 
 * obtained and you should wait until it receives one.
 * In the main loop of your program you need to call dhcp_renew to ensure
 * periodic renewal of addresses (lease time). When the lease time is expired,
 * it will send a renew packet and handle the answer like done in
 * dhcp_request_ip.
 *
 * <h4>Without renewal of lease</h4>
 * You can receive an initial IP address like described above, except there
 * will be no retry on failure. Neither will there be a lease renewal when it
 * expires.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.9.0
 */

#ifndef NET_DHCP_H
#define NET_DHCP_H

#include "../config.h"

// Do we want DHCP?
#ifdef NET_DHCP



#endif // NET_DHCP
#endif // NET_DHCP_H

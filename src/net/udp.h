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
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.7.0
 */

#ifndef NET_UDP_H
#define NET_UDP_H

#include "../config.h"

// Do we want UDP?
#ifdef NET_UDP

#include <inttypes.h>
#include "shared.h"
#include "../utils/logger.h"
#include "../utils/port_service.h"
#include "../utils/werkti.h"

/**
 * @brief Prepare the headers (ethernet, ip, udp) for a packet.
 *
 * Add data at the returned pointer and send the packet by using
 * udp_send(length).
 *
 * @param port_source Port which the packet is send from
 * @param ip_destination IP address the packet is send to
 * @param port_destination Port which the packet is send to
 * @param mac_destination MAC address the packet is send to
 * @return Pointer to the start of the data block to write to
 */
extern uint8_t *udp_prepare(uint16_t port_source, uint8_t *ip_destination, uint16_t port_destination, uint8_t *mac_destination);

/**
 * @brief Send a prepared packet of the provided length.
 *
 * The length provided should be the length of the data of the packet. The
 * length in the headers (ip, udp) will be set accordingly and checksums
 * calculated. Afterwards the packet will be transfered to the network chip and
 * transmitted.
 *
 * @param length Length of the data block
 */
extern void udp_send(uint16_t length);

// Do we want UDP server?
#ifdef NET_UDP_SERVER

/**
 * @brief Initialize the UDP server.
 *
 * @note Should not be called by users, it is called in the initialization of
 * the network chip.
 */
extern void udp_server_init(void);

/**
 * @brief Handle received UDP packets.
 *
 * Looks up in the port service table if the packet was send to a port which has
 * a service attached. If so, the service will be called and given a pointer to
 * the start of the data and the length of the data.
 */
extern void udp_receive(void);

/**
 * @brief Register a service to a port.
 *
 * @param port Port to register to
 * @param callback Callback function which takes (uint8_t *data, uint16_t length)
 */
extern void udp_port_register(uint16_t port, void (*callback)(uint8_t *data, uint16_t length));

/**
 * @brief Unregister a service from a port
 *
 * @param port Port to unregister from
 */
extern void udp_port_unregister(uint16_t port);

/**
 * @brief Create an UDP reply template from a received UDP packet.
 *
 * Returns the pointer to the start of the data block. <br />
 *
 * @note Use udp_send(length) to send the packet.
 * @return Pointer to the start of the data block
 */
extern uint8_t *udp_prepare_reply(void);

#endif // NET_UDP_SERVER
#endif // NET_UDP
#endif // NET_UDP_H

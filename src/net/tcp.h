/**
 * @file tcp.h
 * @brief Transmission Control Protocol functionality
 *
 * This contains the prototype for all TCP functionality which is global
 * available. It allows you to register services to ports, prepare TCP packets
 * and send them. If you use the prototype functions like tcp_prepare,
 * tcp_prepare_reply and tcp_send, all headers will be set correctly and
 * checksums will be calculated and added to the packet.
 *
 * \copyright Copyright 2014 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.14.0
 */

#ifndef NET_TCP_H
#define NET_TCP_H

#include "../config.h"

// Do we want TCP?
#ifdef NET_TCP

#include <inttypes.h>
#include "shared.h"
#include "../utils/logger.h"
#include "../utils/port_service.h"
#include "../utils/werkti.h"

#define tcp_add_flags(x) buffer_out[TCP_PTR_FLAGS] = x

/**
 * @brief Prepare the headers (ethernet, ip, tcp) for a packet.
 *
 * Add data at the returned pointer and send the packet by using
 * tcp_send(length).
 *
 * @param port_source Port which the packet is send from
 * @param ip_destination IP address the packet is send to
 * @param port_destination Port which the packet is send to
 * @param mac_destination MAC address the packet is send to
 * @return Pointer to the start of the data block to write to
 */
extern uint8_t *tcp_prepare();

/**
 * @brief Send a prepared packet of the provided length.
 *
 * The length provided should be the length of the data of the packet. The
 * length in the headers (ip, tcp) will be set accordingly and checksums
 * calculated. Afterwards the packet will be transfered to the network chip and
 * transmitted.
 *
 * @param length Length of the data block
 */
extern void tcp_send();

// Do we want TCP server?
#ifdef NET_TCP_SERVER

/**
 * @brief Initialize the TCP server.
 *
 * @note Should not be called by users, it is called in the initialization of
 * the network chip.
 */
extern void tcp_server_init(void);

/**
 * @brief Handle received TCP packets.
 *
 * Looks up in the port service table if the packet was send to a port which has
 * a service attached. If so, the service will be called and given a pointer to
 * the start of the data and the length of the data.
 */
extern void tcp_receive(void);

/**
 * @brief Register a service to a port.
 *
 * @param port Port to register to
 * @param callback Callback function which takes (uint8_t *data, uint16_t length)
 */
extern void tcp_port_register(uint16_t port, void (*callback)(uint8_t *data, uint16_t length));

/**
 * @brief Unregister a service from a port
 *
 * @param port Port to unregister from
 */
extern void tcp_port_unregister(uint16_t port);

/**
 * @brief Create an TCP reply template from a received TCP packet.
 *
 * Returns the pointer to the start of the data block. <br />
 *
 * @note Use tcp_send(length) to send the packet.
 * @return Pointer to the start of the data block
 */
extern uint8_t *tcp_prepare_reply(void);

#endif // NET_TCP_SERVER
#endif // NET_TCP
#endif // NET_TCP_H

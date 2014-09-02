/**
 * @file network.h
 * @brief Network communication using network chip ENC28J60.
 * Initialization before main loop and handling during main loop.
 *
 * Two important functions are __network_init(void)__ and
 * __network_backbone(void)__. These regulate most network traffic. Before your
 * main loop starts you should call <i>network_init(void)</i>, this initializes
 * the network chip and sets values like MAC address (and IP address if you do
 * not want DHCP). In your main loop you should call _network_backbone(void)_,
 * this sends the packets to the correct protocol handlers (if they are enabled
 * in the config).
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.3.0
 */

#ifndef NET_NETWORK_H
#define NET_NETWORK_H

#include "../config.h"

// Do we want network?
#ifdef NET_NETWORK

// Make sure SPI is enabled
#ifndef COM_SPI
#warning Using NET_NETWORK while COM_SPI not enabled, forcing COM_SPI
#define COM_SPI
#endif // COM_SPI

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include "network_defines.h"
#include "arp.h"
#include "icmp.h"
#include "tcp.h"
#include "udp.h"
#include "../com/spi.h"
#include "../utils/counter.h"
#include "../utils/logger.h"
#include "../utils/werkti.h"

/**
 * @brief Initialization of the network chip, includes setting MAC address (and
 * IP address should be set if DHCP is disabled)
 *
 * Initialization of the network chip is done by this function. It should be
 * called before the main loop starts and will set important functions on the
 * network chip. MAC address is always set, IP address should be set when DHCP
 * is disabled.
 *
 * @see #network_backbone(void)
 */
extern void network_init(void);

/**
 * @brief Handling of receiving packets.
 *
 * This function handles incoming packets, it should be the first call in your
 * main loop. It looks at a packet to see which protocol it is and calls the
 * handler associated to it. When no handler is found for the packet, the packet
 * is not discarded but should be handled manually in the main loop.
 *
 * @note If a packet is smaller than the required 60 bytes of an ethernet frame
 * it will be zero padded to match 60 bytes. This reflects in the length set in
 * _buffer_in_length_. If the length is available in a protocol header,
 * use that length to know the actual length of the data block.
 *
 * @see #network_init(void)
 */
extern void network_backbone(void);

/**
 * @brief Is the network chip connected to a network?
 *
 * @return One if there is a connection
 */
extern uint8_t network_is_link_up(void);

/**
 * @brief Return the status of the physical layer
 *
 * <table>
 *   <tr>
 *     <td>7</td>
 *     <td>6</td>
 *     <td>5</td>
 *     <td>4</td>
 *     <td>3</td>
 *     <td>2</td>
 *     <td>1</td>
 *     <td>0</td>
 *   </tr>
 *   <tr>
 *     <td>TXSTAT</td>
 *     <td>RXSTAT</td>
 *     <td>COLSTAT</td>
 *     <td>LSTAT</td>
 *     <td>DXSTAT</td>
 *     <td>LLSTAT</td>
 *     <td>JBSTAT</td>
 *     <td>PLRITY</td>
 *   </tr>
 * </table>
 * Explanation: <br />
 * TXSTAT:  Transmit status (1 = transmitting, 0 = not transmitting)<br />
 * RXSTAT:  Receive status (1 = receiving, 0 = not receiving)<br />
 * COLSTAT: Collision status (1 = collision occuring, 0 = no collision)<br />
 * LSTAT:   Link status (1 = link is up, 0 = link is down)<br />
 * DXSTAT:  Duplex status (1 = full-duplex, 0 = half-duplex)<br />
 * LLSTAT:  Latching link status (1 = up and continious, 0 = down or was down)<br />
 * JBSTAT:  Latching jabber status (1 = jabber criterea met, 0 = no jabbering)<br />
 * PLRITY:  Polarity status (1 = TPIN+/- is reversed, 0 = TPIN+/- is correct)<br />
 *
 * @return Bitmask of current status, see description for bitmask
 */
extern uint8_t network_status(void);

/**
 * @brief Send a packet from _buffer_out_ to the network chip to the
 * connected network
 *
 * When you have created a packet in _buffer_out_ you can send it by
 * calling this function. The length parameter is the length of the total paket,
 * including all headers.
 *
 * @param length The total length of the packet to be send
 */
extern void network_send(uint16_t length);

/**
 * @brief Enable broadcast packets on the network chip
 */
extern void network_broadcast_enable(void);

/**
 * @brief Disable broadcast packets on the network chip
 */
extern void network_broadcast_disable(void);

/**
 * @brief Received packet buffer
 *
 * This buffer contains a received packet. It holds the complete packet and not
 * just the data. This includes the ethernet header.
 */
extern uint8_t buffer_in[];

/**
 * @brief Transmit packet buffer
 *
 * This buffer is for building a packet for transmission. It should have the
 * whole packet including ethernet header.
 */
extern uint8_t buffer_out[];

/**
 * @brief Length of the received packet
 *
 * This holds the total length of a received packet. This includes the ethernet
 * header.
 *
 * @note If a packet is smaller than the required 60 bytes of an ethernet frame
 * it will be zero padded to match 60 bytes. This reflects in the length set in
 * _buffer_in_length_. If the length is available in a protocol header,
 * use that length to know the actual length of the data block.
 */
extern uint16_t buffer_in_length;

#endif // NET_NETWORK
#endif // NET_NETWORK_H

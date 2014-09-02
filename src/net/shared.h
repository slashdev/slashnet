/**
 * @file shared.h
 * @brief Shared resources, like pointers and values of ethernet related
 * material within packets.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 1.0
 */

#ifndef NET_SHARED_H
#define NET_SHARED_H

#include "../config.h"

// Do we need shared?
#ifdef NET_NETWORK

#include <inttypes.h>
#include "network.h"

/**
 * @brief Prepare an ETH and IP header in bufferOut
 *
 * This function is used by udp_prepare and tcp_prepare to create the headers
 * of a packet to send. It sets the source and destination addresses, protocol
 * type, version, type of service (ToS), identification, flags, time to live
 * (TTL), protocol and source and destination IP addresses.
 *
 * @param protocol Protocol to be used, see RFC 1700, p. 8, PROTOCOL NUMBERS
 * @param dst_ip IP address of the destination
 * @param dst_mac MAC address of the destination
 * @see #udp_prepare(uint16_t, uint8_t*, uint16_t, uint8_t*)
 * @see #udp_prepare_reply(void)
 */
extern void ip_prepare(uint8_t protocol, uint8_t *dst_ip, uint8_t *dst_mac);
/**
 * @brief Calculate the checksum of the provided buffer for the provided length
 *
 * @param buffer Pointer from where the checksum should be calculated
 * @param length Length of the buffer of which the checksum is to be calculated
 * @param type Type of packet, defined in CHK_*
 * @return Calculated checksum
 */
extern uint16_t checksum(uint8_t *buffer, uint16_t length, uint8_t type);

/**
 * @brief Add value to a number in buffer
 *
 * @param value Value to add
 * @param buffer Pointer to the highest byte of the number in the buffer
 * @param size Number of bytes that make up the number
 */
extern void add_value_to_buffer(uint16_t value, uint8_t *buff, uint8_t size);

// Checksum
// --------------------
#define CHK_IP   0
#define CHK_ICMP 0
#define CHK_UDP  1
#define CHK_TCP  2


// ETH
// --------------------
#define ETH_LEN_HEADER 14

// See The Ethernet, p. 26, chap. 6.2
// See for ethernet frame http://en.wikipedia.org/wiki/Ethernet_frame
#define ETH_PTR_MAC_DST    0x00
#define ETH_PTR_MAC_SRC    0x06
#define ETH_PTR_TYPE_H     0x0C
#define ETH_PTR_TYPE_L     0x0D

// See for EtherTypes http://en.wikipedia.org/wiki/Ethertype
#define ETH_VAL_TYPE_ARP_H 0x08
#define ETH_VAL_TYPE_ARP_L 0x06
#define ETH_VAL_TYPE_IP_H  0x08
#define ETH_VAL_TYPE_IP_L  0x00


// ARP
// --------------------
#define ARP_LEN 42

#define ARP_PTR_HARDWARE_H 0x0E
#define ARP_PTR_HARDWARE_L 0x0F
#define ARP_PTR_PROTOCOL_H 0x10
#define ARP_PTR_PROTOCOL_L 0x11
#define ARP_PTR_SIZE_HW    0x12
#define ARP_PTR_SIZE_PROTO 0x13
#define ARP_PTR_OPER_H     0x14
#define ARP_PTR_OPER_L     0x15
#define ARP_PTR_SEND_HW    0x16
#define ARP_PTR_SEND_PROTO 0x1C
#define ARP_PTR_TARG_HW    0x20
#define ARP_PTR_TARG_PROTO 0x26

#define ARP_VAL_HW_ETHERNET 1

// Protocol type uses same values as ETH_VAL_TYPE_*

#define ARP_VAL_OPER_REQUEST 1
#define ARP_VAL_OPER_REPLY   2


// IP
// --------------------
#define IP_LEN_HEADER 20

#define IP_PTR             0x0E
#define IP_PTR_HEADER_LEN  0x0E
#define IP_PTR_TOS         0x0F
#define IP_PTR_LENGTH_H    0x10
#define IP_PTR_LENGTH_L    0x11
#define IP_PTR_ID_H        0x12
#define IP_PTR_ID_L        0x13
#define IP_PTR_FLAGS       0x14
#define IP_PTR_FRAGMENT_L  0x15
#define IP_PTR_TTL         0x16
#define IP_PTR_PROTOCOL    0x17
#define IP_PTR_CHECKSUM_H  0x18
#define IP_PTR_CHECKSUM_L  0x19
#define IP_PTR_SRC         0x1A
#define IP_PTR_DST         0x1E

// Protocol values
// See RFC 1700, p. 8
#define IP_VAL_PROTO_ICMP  0x01
#define IP_VAL_PROTO_TCP   0x06
#define IP_VAL_PROTO_UDP   0x11


// ICMP
// --------------------
#define ICMP_LEN_HEADER 8

#define ICMP_PTR_TYPE      0x22
#define ICMP_PTR_CODE      0x23
#define ICMP_PTR_CHECKSUM_H 0x24
#define ICMP_PTR_CHECKSUM_L 0x25
#define ICMP_PTR_IDENT_H   0x26
#define ICMP_PTR_IDENT_L   0x27
#define ICMP_PTR_SEQ_H     0x28
#define ICMP_PTR_SEQ_L     0x29
#define ICMP_PTR_DATA      0x2A

#define ICMP_VAL_TYPE_ECHOREPLY   0x00
#define ICMP_VAL_TYPE_ECHOREQUEST 0x08


// UDP
// --------------------
#define UDP_LEN_HEADER 8

#define UDP_PTR_PORT_SRC_H 0x22
#define UDP_PTR_PORT_SRC_L 0x23
#define UDP_PTR_PORT_DST_H 0x24
#define UDP_PTR_PORT_DST_L 0x25
#define UDP_PTR_LENGTH_H   0x26
#define UDP_PTR_LENGTH_L   0x27
#define UDP_PTR_CHECKSUM_H 0x28
#define UDP_PTR_CHECKSUM_L 0x29
#define UDP_PTR_DATA       0x2A

// TCP
// --------------------
#define TCP_LEN_HEADER 20

#define TCP_PTR_PORT_SRC_H 0x22
#define TCP_PTR_PORT_SRC_L 0x23
#define TCP_PTR_PORT_DST_H 0x24
#define TCP_PTR_PORT_DST_L 0x25
#define TCP_PTR_SEQ_NR     0x26
#define TCP_PTR_ACK_NR     0x2A
#define TCP_PTR_DATA_OFFSET 0x2E
#define TCP_PTR_FLAGS      0x2F
#define TCP_PTR_WINDOW     0x30
#define TCP_PTR_CHECKSUM_H 0x32
#define TCP_PTR_CHECKSUM_L 0x33
#define TCP_PTR_URGENT_PTR 0x34
#define TCP_PTR_OPTIONS    0x36
#define TCP_PTR_DATA       0x36
#define TCP_PTR_DATA_OPTS  0x3E

#define TCP_FLAG_CWR       0b10000000
#define TCP_FLAG_ECN_ECHO  0b01000000
#define TCP_FLAG_URGENT    0b00100000
#define TCP_FLAG_ACK       0b00010000
#define TCP_FLAG_PUSH      0b00001000
#define TCP_FLAG_RESET     0b00000100
#define TCP_FLAG_SYN       0b00000010
#define TCP_FLAG_FIN       0b00000001

#endif // NET_NETWORK
#endif // NET_SHARED_H

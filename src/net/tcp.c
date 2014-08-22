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

uint8_t sequence_nr = 1;

uint8_t *tcp_prepare(uint16_t src_port, uint8_t *dst_ip, uint16_t dst_port, uint8_t *dst_mac) {
    // Create IP protocol header
    ip_prepare(IP_VAL_PROTO_TCP, dst_ip, dst_mac);

    // Construct TCP protocol header
    // -----------------------------
    // See RFC 793, p. 15
    uint8_t *buff = &buffer_out[TCP_PTR_PORT_SRC_H];
    // Source port [TCP_PTR_PORT_SRC_H]
    *buff++ = src_port >> 8;
    *buff++ = src_port & 0xFF;
    // Destination port [TCP_PTR_PORT_DST_H]
    *buff++ = dst_port >> 8;
    *buff++ = dst_port & 0xFF;
    // Sequence number [TCP_PTR_SEQ_NR]
    *buff++ = 1;
    *buff++ = 0;
    *buff++ = 0;
    *buff++ = sequence_nr++;
    // Acknowledgement number [TCP_PTR_ACK_NR]
    *buff++ = 0;
    *buff++ = 0;
    *buff++ = 0;
    *buff++ = 0;
    // Header length [TCP_PTR_DATA_OFFSET]
    // Pre options: 5 x 32 bits
    // Options:     2 x 32 bits
    *buff++ = 0x07 << 4;
    // Flags: Only set SYN [TCP_PTR_FLAGS]
    *buff++ = TCP_FLAG_SYN;
    // Window: 1024 bytes max (0x400) [TCP_PTR_WINDOW]
    *buff++ = 0;
    *buff++ = 0;
    *buff++ = 0x04;
    *buff++ = 0;
    // Checksum: set to 0 [TCP_PTR_CHECKSUM_H]
    *buff++ = 0;
    *buff++ = 0;
    // Urgent pointer: set to 0 [TCP_PTR_URGENT_H]
    *buff++ = 0;
    *buff++ = 0;
    // Options: [TCP_PTR_OPTIONS]
    // Maximum segment size: 1024 (0x400)
    *buff++ = 0x02;
    *buff++ = 0x04;
    *buff++ = 0x04;
    *buff++ = 0x00;
    // Nop to fill for next option
    //buffer_out[TCP_PTR_OPTIONS+4] = 0x01;
    // Window scale: 0 (no multiplication)
    *buff++ = 0x03;
    *buff++ = 0x03;
    *buff++ = 0x00;
    // End of option list
    *buff++ = 0x00;

    return &buffer_out[TCP_PTR_DATA];
}

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

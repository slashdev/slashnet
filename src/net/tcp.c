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

    // Construct UDP protocol header
    // -----------------------------
    // See RFC 793, p. 15
    // Source port
    buffer_out[TCP_PTR_PORT_SRC_H] = src_port >> 8;
    buffer_out[TCP_PTR_PORT_SRC_L] = src_port & 0xFF;
    // Destination port
    buffer_out[TCP_PTR_PORT_DST_H] = dst_port >> 8;
    buffer_out[TCP_PTR_PORT_DST_L] = dst_port & 0xFF;
    // Sequence number
    buffer_out[TCP_PTR_SEQ_NR+0] = 1;
    buffer_out[TCP_PTR_SEQ_NR+1] = 0;
    buffer_out[TCP_PTR_SEQ_NR+2] = 0;
    buffer_out[TCP_PTR_SEQ_NR+3] = sequence_nr++;
    // Acknowledgement number
    buffer_out[TCP_PTR_ACK_NR+0] = 0;
    buffer_out[TCP_PTR_ACK_NR+1] = 0;
    buffer_out[TCP_PTR_ACK_NR+2] = 0;
    buffer_out[TCP_PTR_ACK_NR+3] = 0;
    // Header length (includes options)
    // Pre options: 5 x 32 bits
    // Options:     2 x 32 bits
    buffer_out[TCP_PTR_DATA_OFFSET] = 0x07 << 4;
    // Flags: Only set SYN
    buffer_out[TCP_PTR_FLAGS] = TCP_FLAG_SYN;
    // Window: 1024 bytes max (0x400)
    buffer_out[TCP_PTR_WINDOW+0] = 0;
    buffer_out[TCP_PTR_WINDOW+1] = 0;
    buffer_out[TCP_PTR_WINDOW+2] = 0x04;
    buffer_out[TCP_PTR_WINDOW+3] = 0;
    // Checksum: set to 0
    buffer_out[TCP_PTR_CHECKSUM+0] = 0;
    buffer_out[TCP_PTR_CHECKSUM+1] = 0;
    // Urgent pointer: set to 0
    buffer_out[TCP_PTR_URGENT_PTR+0] = 0;
    buffer_out[TCP_PTR_URGENT_PTR+1] = 0;
    // Options:
    // Maximum segment size: 1024 (0x400)
    buffer_out[TCP_PTR_OPTIONS+0] = 0x02;
    buffer_out[TCP_PTR_OPTIONS+1] = 0x04;
    buffer_out[TCP_PTR_OPTIONS+2] = 0x04;
    buffer_out[TCP_PTR_OPTIONS+3] = 0x00;
    // Nop to fill for next option
    //buffer_out[TCP_PTR_OPTIONS+4] = 0x01;
    // Window scale: 0 (no multiplication)
    buffer_out[TCP_PTR_OPTIONS+4] = 0x03;
    buffer_out[TCP_PTR_OPTIONS+5] = 0x03;
    buffer_out[TCP_PTR_OPTIONS+6] = 0x00;
    // End of option list
    buffer_out[TCP_PTR_OPTIONS+7] = 0x00;

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

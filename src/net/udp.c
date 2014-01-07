/**
 * @file udp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf
 * @since
 */

#include "udp.h"

// Do we want UDP?
#ifdef NET_UDP

// Check if NET_NETWORK is enabled
#ifndef NET_NETWORK
#error UDP cannot work without NET_NETWORK
#endif // NET_NETWORK

// Output buffer from config
extern uint8_t buffer_out[];
// Addresses from config
extern uint8_t my_mac[];
extern uint8_t my_ip[];

uint8_t *udp_prepare(uint16_t src_port, uint8_t *dst_ip, uint16_t dst_port, uint8_t *dst_mac) {
    // Create IP protocol header
    ip_prepare(IP_VAL_PROTO_UDP, dst_ip, dst_mac);
    
    // Construct UDP protocol header
    // -----------------------------
    // See RFC 768, p. 1
    // Source port
    buffer_out[UDP_PTR_PORT_SRC_H] = src_port >> 8;
    buffer_out[UDP_PTR_PORT_SRC_L] = src_port & 0xFF;
    // Destination port
    buffer_out[UDP_PTR_PORT_DST_H] = dst_port >> 8;
    buffer_out[UDP_PTR_PORT_DST_L] = dst_port & 0xFF;
    // Length: ignore, set correctly in upd_send
    // Checksum: 0, set correctly in udp_send
    buffer_out[UDP_PTR_CHECKSUM_H] = 0;
    buffer_out[UDP_PTR_CHECKSUM_L] = 0;
    // Return pointer to the data block
    return &buffer_out[UDP_PTR_DATA];
}

void udp_send(uint16_t length) {
    
    uint16_t tmp;
    
    // IP protocol packet length
    tmp = IP_LEN_HEADER + UDP_LEN_HEADER + length;
    buffer_out[IP_PTR_LENGTH_H] = tmp >> 8;
    buffer_out[IP_PTR_LENGTH_L] = tmp & 0xFF;
    
    // UDP protocol packet length
    tmp = UDP_LEN_HEADER + length;
    buffer_out[UDP_PTR_LENGTH_H] = tmp >> 8;
    buffer_out[UDP_PTR_LENGTH_L] = tmp & 0xFF;
    
    // Calculate checksum IP header
    tmp = checksum(&buffer_out[IP_PTR], IP_LEN_HEADER, CHK_IP);
    buffer_out[IP_PTR_CHECKSUM_H] = tmp >> 8;
    buffer_out[IP_PTR_CHECKSUM_L] = tmp & 0xFF;
    
    // Calculate checksum UDP data
    tmp = checksum(&buffer_out[IP_PTR_SRC], 16 + length, CHK_UDP);
    buffer_out[UDP_PTR_CHECKSUM_H] = tmp >> 8;
    buffer_out[UDP_PTR_CHECKSUM_L] = tmp & 0xFF;
    
#ifdef UTILS_WERKTI_EXTENDED
    // Update werkti udp out
    werkti_udp_out += ETH_LEN_HEADER + IP_LEN_HEADER + UDP_LEN_HEADER + length;
#endif // API_WERKTI_EXTENDED
    
    // Send packet to chip
    network_send(ETH_LEN_HEADER + IP_LEN_HEADER + UDP_LEN_HEADER + length);
}

#endif // NET_UDP

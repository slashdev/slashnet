/**
 * @file shared.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 1.0
 */

#include "shared.h"

// Do we need shared?
#ifdef NET_NETWORK

// Identifier used in IP protocol header
volatile uint8_t id_nr = 0x05;

// Functions
// ---------

void ip_prepare(uint8_t protocol, uint8_t *dst_ip, uint8_t *dst_mac) {
    uint8_t i = 0;

    // Construct ethernet frame
    // ------------------------
    // See The Ethernet, p. 26, chap. 6.2
    // 6b  mac address destination
    // 6b  mac address source
    // 2b  protocol (here IP, so 0x800)

    // Copy mac source and destination address into buffer
    while (i < 6) {
        buffer_out[ETH_PTR_MAC_DST + i] = dst_mac[i];
        buffer_out[ETH_PTR_MAC_SRC + i] = my_mac[i];
        i++;
    }

    // Set packet type to IP
    buffer_out[ETH_PTR_TYPE_H] = ETH_VAL_TYPE_IP_H;
    buffer_out[ETH_PTR_TYPE_L] = ETH_VAL_TYPE_IP_L;

    // Construct IP protocol header
    // ----------------------------
    // See RFC 791, p. 11, chap. 3.1
    // Version: 4, IHL: 5
    buffer_out[IP_PTR] = 0x45;
    // Leave ToS 0: Routine, normal delay, normal throughput, normal reliability
    buffer_out[IP_PTR_TOS] = 0;
    // Total length: 0, should be set before sending
    buffer_out[IP_PTR_LENGTH_H] = 0;
    buffer_out[IP_PTR_LENGTH_L] = 0;
    // Identification: use ipIdentification
    buffer_out[IP_PTR_ID_L] = id_nr;
    id_nr++;
    // Flags: "Don't Fragment", "Last Fragment" (010), fragment offset 0
    buffer_out[IP_PTR_FLAGS] = 0x40;
    // Fragment: 0, we don't fragment
    buffer_out[IP_PTR_FRAGMENT_L] = 0;
    // Time to live: 32 hops, so 0x20
    buffer_out[IP_PTR_TTL] = 0x40;
    // Protocol
    buffer_out[IP_PTR_PROTOCOL] = protocol;
    // Header checksum: 0, for checksum calculation this needs to be zero
    buffer_out[IP_PTR_CHECKSUM_H] = 0;
    buffer_out[IP_PTR_CHECKSUM_L] = 0;
    // Source address, destination address
    i = 0;
    while (i < 4) {
        buffer_out[IP_PTR_DST + i] = dst_ip[i];
        buffer_out[IP_PTR_SRC + i] = my_ip[i];
        i++;
    }
}

// Calculate checksum of packages
// Set the checksum fields to 0 before running the checksum
uint16_t checksum(uint8_t *buffer, uint16_t length, uint8_t type) {
    uint32_t sum = 0;

    // Add protocol value and length
    switch (type) {
        case CHK_IP:
            break;
        case CHK_UDP:
            // Add protocol value: 17 (UDP)
            sum += IP_VAL_PROTO_UDP;
            // Add UDP length: length - IP source and destination
            sum += length - 8;
            break;
        case CHK_TCP:
            // Add protocol value: 6
            sum += IP_VAL_PROTO_TCP;
            // Add TCP length: length - IP source and destination
            sum += length - 8;
            break;
    }

    // Process the buffer for length in 16 bits words
    while (length > 1) {
        sum += 0xFFFF & ((((uint32_t)*buffer) << 8) | *(buffer + 1));
        buffer += 2;
        length -= 2;
    }

    // If there is a byte left, add it with padding
    if (length) {
        sum += ((uint32_t)(0xFF & *buffer)) << 8;
    }

    // If the sum is over 16 bits, calculate the sum over the bytes until the
    // result is a 16 bits word
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Return 1's complement
    return ((uint16_t)sum ^ 0xFFFF);
}

// Add a value to a number in a buffer
void add_value_to_buffer(uint16_t value, uint8_t *buff, uint8_t size) {
    buff += size - 1;
    // While there is a value and we have size left
    while(value > 0 && size-- > 0) {
        // Add value of current buffer spot
        value += *buff;
        // Place lower byte back in buffer
        *buff = value & 0xFF;
        // Move buffer and value
        value = value >> 8;
        buff--;
    }
}

#endif // NET_NETWORK

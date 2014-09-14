/**
 * @file icmp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.8.0
 */

#include "icmp.h"

// Do we want ICMP?
#ifdef NET_ICMP

void icmp_ping_reply();

void icmp_packet_receive() {

#ifdef UTILS_WERKTI_MORE
    // Update werkti icmp in
    werkti_icmp_in += buffer_in_length;
#endif // UTILS_WERKTI_MORE

    uint8_t type = buffer_in[ICMP_PTR_TYPE];

    if(type == ICMP_VAL_TYPE_ECHOREQUEST) {
        debug_string_p(PSTR("ICMP: Request - "));
        icmp_ping_reply();
        debug_string_p(PSTR("sent\r\n"));
        // Packet handled, reset length
        buffer_in_length = 0;
    }

}

void icmp_ping_reply() {
    uint8_t i = 0;

    // Send back the packet we get, except for some minor changes.
    // - Swap sender and receiver in ETH header
    // - Swap sender and receiver in IP header
    // - Change ICMP type from request (8) to reply (0)
    // - Update ICMP checksum

    // Copy buffer
    while (i < buffer_in_length) {
        buffer_out[i] = buffer_in[i];
        i++;
    }

    // Swap sender and receiver in ETH header
    // Use own MAC address for sender
    i = 0;
    while (i < 6) {
        buffer_out[ETH_PTR_MAC_DST + i] = buffer_in[ETH_PTR_MAC_SRC + i];
        buffer_out[ETH_PTR_MAC_SRC + i] = my_mac[i];
        i++;
    }

    // Swap sender and receiver in IP header
    i = 0;
    while (i < 4) {
        buffer_out[IP_PTR_DST + i] = buffer_in[IP_PTR_SRC + i];
        buffer_out[IP_PTR_SRC + i] = my_ip[i];
        i++;
    }

    // Change ICMP type from request (8) to reply (0)
    buffer_out[ICMP_PTR_TYPE] = ICMP_VAL_TYPE_ECHOREPLY;

    // Update ICMP header checksum
    // Since only the type of the packet is changed compared to the received
    // packet, we can copy the checksum and add 0x08 to it. This is correct
    // since it's 1st complement.
    buffer_out[ICMP_PTR_CHECKSUM_H] += 0x08;
    if (buffer_in[ICMP_PTR_CHECKSUM_H] > (0xFF - 0x08)) {
        buffer_out[ICMP_PTR_CHECKSUM_L]++;
    }

#ifdef UTILS_WERKTI_MORE
    // Update werkti icmp out
    werkti_icmp_out += buffer_in_length;
#endif // UTILS_WERKTI_MORE

    // Send packet to chip
    network_send(buffer_in_length);
}

#endif // NET_ICMP

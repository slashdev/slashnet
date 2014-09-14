/**
 * @file arp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.8.0
 */

#include "arp.h"

// Do we want ARP?
#ifdef NET_ARP

#ifndef NET_ARP_CACHE_SIZE
#error ARP cannot function without NET_ARP_CACHE_SIZE defined
#endif // NET_ARP_CACHE_SIZE

// Types
// -----

// Definition of an arpItem
typedef struct {
    uint8_t ip[4];
    uint8_t mac[6];
} arp_item_t;

// Variables
// ---------

// Arp cache
arp_item_t cache[NET_ARP_CACHE_SIZE];
// Next write index for a reply to a request
volatile uint8_t cache_index = 0;
// Are we waiting for a reply on a request
volatile uint8_t waiting = 0;

#ifdef UTILS_WERKTI_MORE
// Traffic from werkti
extern uint16_t werkti_arp_in;
extern uint16_t werkti_arp_out;
#endif // UTILS_WERKTI_MORE

// Functions
// ---------

void arp_reply_to_request(void);
void arp_prepare(uint8_t *dst_mac);
void save_to_cache(void);
uint8_t *arp_search_mac(uint8_t *ip_request);

void arp_init(void) {
    uint8_t i = 0, j = 0;

    // Initialize elements from arpItems
    while (i < NET_ARP_CACHE_SIZE) {
        j = 0;
        while (j < 4) {
            cache[i].ip[j]  = 0;
            cache[i].mac[j] = 0;
            j++;
        }
        while (j < 6) {
            cache[i].mac[j] = 0;
            j++;
        }
        i++;
    }
}

void arp_receive(void) {
#ifdef UTILS_WERKTI_MORE
    // Update arp incoming
    werkti_arp_in += buffer_in_length;
#endif // UTILS_WERKTI_MORE

    uint8_t i = 0;

    // If it is an ARP request packet, it could be requesting my MAC address
    if (buffer_in[ARP_PTR_OPER_H] == 0
        && buffer_in[ARP_PTR_OPER_L] == ARP_VAL_OPER_REQUEST) {
        // Is the packet for me?
        while (i < 4) {
            if (buffer_in[ARP_PTR_TARG_PROTO + i] != my_ip[i]) {
                return; // Not my IP address
            }
            i++;
        }

        // It's meant for me, reply with my MAC address
        debug_string_p(PSTR("ARP: Request for me..."));
        arp_reply_to_request();
        debug_string_p(PSTR("answered\r\n"));

        // Packet handled, reset buffer length
        buffer_in_length = 0;
        return;
    }


    // If it is an ARP reply packet, it could be an answer to me
    if (buffer_in[ARP_PTR_OPER_H] == 0
        && buffer_in[ARP_PTR_OPER_L] == ARP_VAL_OPER_REPLY) {
        // Is the packet for me?
        while (i < 4) {
            if (buffer_in[ARP_PTR_TARG_PROTO + i] != my_ip[i]) {
                return;
            }
            i++;
        }

        // It's meant for me, store value
        debug_string_p(PSTR("ARP: Reply for me "));
        save_to_cache();
        debug_string_p(PSTR("saved\r\n"));

        // Reset waiting state
        waiting = 0;

        // Packet handled, reset buffer length
        buffer_in_length = 0;
        return;
    }
}

uint8_t *arp_request_mac(uint8_t *ip_request) {
    // Check cache for ARP entry of ipRequest
    uint8_t *mac_answer = arp_search_mac(ip_request);
    // If we have an entry, return the result
    if (mac_answer != 0) {
        return mac_answer;
    }

    // No entry existed, create a request and send it
    uint8_t i = 0,
    all_FF[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    // Create a new ARP packet
    arp_prepare(all_FF);

    // Set operation type
    buffer_out[ARP_PTR_OPER_H] = 0;
    buffer_out[ARP_PTR_OPER_L] = ARP_VAL_OPER_REQUEST;

    // Clear the target MAC address
    // Set the target IP address
    while (i < 4) {
        buffer_out[ARP_PTR_TARG_HW + i] = 0;
        buffer_out[ARP_PTR_TARG_PROTO + i] = ip_request[i];
        i++;
    }
    while (i < 6) {
        buffer_out[ARP_PTR_TARG_HW + i] = 0;
        i++;
    }

#ifdef UTILS_WERKTI_MORE
    // Update werkti arp send
    werkti_arp_out += ARP_LEN;
#endif // UTILS_WERKTI_MORE

    network_send(ARP_LEN);

    // Wait actively for an answer
    waiting = 1;
    while (waiting) {
        // Process packets received
        network_backbone();
    }

    // It should exist now
    return arp_search_mac(ip_request);
}

void arp_reply_to_request(void) {
    uint8_t i = 0;

    // Create ARP template based on request
    arp_prepare(&buffer_in[ARP_PTR_SEND_HW]);

    // Fill the missing parts of the packet:
    // Operation, Sender hardware address, sender protocol address,
    // target hardware address, target protocol address

    // Operation: reply
    buffer_out[ARP_PTR_OPER_H] = 0;
    buffer_out[ARP_PTR_OPER_L] = ARP_VAL_OPER_REPLY;

    // Target hardware address
    while (i < 6) {
        buffer_out[ARP_PTR_TARG_HW + i] = buffer_in[ARP_PTR_SEND_HW + i];
        i++;
    }

    // Target protocol address
    i = 0;
    while (i < 4) {
        buffer_out[ARP_PTR_TARG_PROTO + i] = buffer_in[ARP_PTR_SEND_PROTO + i];
        i++;
    }

#ifdef UTILS_WERKTI_MORE
    // Update werkti arp send
    werkti_arp_out += ARP_LEN;
#endif // UTILS_WERKTI_MORE

    // Send reply
    network_send(ARP_LEN);
}

void arp_prepare(uint8_t *dst_mac) {
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

    // Set packet type to ARP
    buffer_out[ETH_PTR_TYPE_H] = ETH_VAL_TYPE_ARP_H;
    buffer_out[ETH_PTR_TYPE_L] = ETH_VAL_TYPE_ARP_L;

    // Construct ARP protocol packet
    // -----------------------------
    // See RFC 826, p. 3
    // Hardware type
    buffer_out[ARP_PTR_HARDWARE_H] = 0;
    buffer_out[ARP_PTR_HARDWARE_L] = ARP_VAL_HW_ETHERNET;
    // Protocol type (we only work with IP, so 0x0800)
    buffer_out[ARP_PTR_PROTOCOL_H] = ETH_VAL_TYPE_IP_H;
    buffer_out[ARP_PTR_PROTOCOL_L] = ETH_VAL_TYPE_IP_L;
    // Hardware size: 6 (MAC address)
    buffer_out[ARP_PTR_SIZE_HW] = 0x06;
    // Protocol size: 4 (IP address)
    buffer_out[ARP_PTR_SIZE_PROTO] = 0x04;
    // Sender MAC address
    i = 0;
    while (i < 4) {
        buffer_out[ARP_PTR_SEND_HW + i] = my_mac[i];
        buffer_out[ARP_PTR_SEND_PROTO + i] = my_ip[i];
        i++;
    }
    while (i < 6) {
        buffer_out[ARP_PTR_SEND_HW + i] = my_mac[i];
        i++;
    }
}

void save_to_cache(void) {
    uint8_t *ip_reply = &buffer_in[ARP_PTR_SEND_PROTO],
    *mac_reply = &buffer_in[ARP_PTR_SEND_HW],
    *ip_cache = cache[cache_index].ip,
    *mac_cache = cache[cache_index].mac,
    i = 0;

    // Search for MAC address to see if the combination already exists in the
    // cache. If it doesn't, write an entry at arpWriteIndex
    if (arp_search_mac(ip_reply) == 0) {
        // Copy IP address and MAC address into cache
        while (i < 4) {
            ip_cache[i] = ip_reply[i];
            mac_cache[i] = mac_reply[i];
            i++;
        }
        while (i < 6) {
            mac_cache[i] = mac_reply[i];
            i++;
        }

        // Update write index
        cache_index++;
        cache_index %= NET_ARP_CACHE_SIZE;
    }

}

uint8_t *arp_search_mac(uint8_t *ip_request) {
    uint8_t i = 0;
    uint8_t *ip_cache;

    while (i < NET_ARP_CACHE_SIZE) {
        ip_cache = cache[i].ip;
        if (ip_cache[0] == ip_request[0]
            && ip_cache[1] == ip_request[1]
            && ip_cache[2] == ip_request[2]
            && ip_cache[3] == ip_request[3]) {
            // IP address matches, return MAC address
            return cache[i].mac;
        }
        i++;
    }

    return 0;
}


#endif // NET_ARP

/**
 * @file udp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.7.0
 */

#include "udp.h"

// Do we want UDP?
#ifdef NET_UDP

// Check if NET_NETWORK is enabled
#ifndef NET_NETWORK
#error UDP cannot work without NET_NETWORK
#endif // NET_NETWORK

// Port services list
#ifdef NET_UDP_SERVER
// Check if port list size is defined
#ifndef NET_UDP_SERVICES_LIST_SIZE
#error NET_UDP_SERVICES_LIST_SIZE not defined, but NET_UDP_SERVER active
#endif // NET_UDP_SERVICES_LIST_SIZE
// Create port service list
port_service_t port_services[NET_UDP_SERVICES_LIST_SIZE];
#endif // NET_UDP_SERVER

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

#ifdef UTILS_WERKTI_MORE
    // Update werkti udp out
    werkti_udp_out += ETH_LEN_HEADER + IP_LEN_HEADER + UDP_LEN_HEADER + length;
#endif // UTILS_WERKTI_MORE

    // Send packet to chip
    network_send(ETH_LEN_HEADER + IP_LEN_HEADER + UDP_LEN_HEADER + length);
}

#ifdef NET_UDP_SERVER

void udp_server_init(void) {
    // Prepare port list
    port_service_init(port_services, NET_UDP_SERVICES_LIST_SIZE);
}

void udp_receive(void) {

#ifdef UTILS_WERKTI_MORE
    // Update werkti udp in
    werkti_udp_in += buffer_in_length;
#endif // UTILS_WERKTI_MORE

    debug_string_p(PSTR("UDP: received\r\n"));

    uint16_t port;
    void (*callback)(uint8_t *data, uint16_t length);

    // Get the port
    port  = ((uint16_t)buffer_in[UDP_PTR_PORT_DST_H]) << 8;
    port |= buffer_in[UDP_PTR_PORT_DST_L];
    debug_string_p(PSTR("UDP: Port: "));
    debug_number(port);
    debug_newline();

    // Check if a listener is registered for this port
    debug_string_p(PSTR("UDP: Search service..."));
    callback = port_service_get(port_services, NET_UDP_SERVICES_LIST_SIZE, port);
    debug_string_p(PSTR("done\r\n"));
    if (callback) {
        debug_string_p(PSTR("UDP: Found callback function\r\n"));
        uint16_t length = ((uint16_t)buffer_in[IP_PTR_LENGTH_H]) << 8;
        length |= buffer_in[IP_PTR_LENGTH_L];
        length -= IP_LEN_HEADER + UDP_LEN_HEADER;
        debug_string_p(PSTR("UDP: Calling callback function\r\n"));
        callback(&buffer_in[UDP_PTR_DATA], length); // Execute callback
        debug_string_p(PSTR("UDP: Callback function returned\r\n"));
    }
    debug_string_p(PSTR("UDP: handled\r\n"));
}

void udp_port_register(uint16_t port, void (*callback)(uint8_t *data, uint16_t length)) {
    port_service_set(port_services, NET_UDP_SERVICES_LIST_SIZE, port, callback);
}

void udp_port_unregister(uint16_t port) {
    port_service_remove(port_services, NET_UDP_SERVICES_LIST_SIZE, port);
}

uint8_t *udp_prepare_reply(void) {
    // Create IP header
    // Create IP protocol header
    ip_prepare(IP_VAL_PROTO_UDP, &buffer_in[IP_PTR_SRC], &buffer_in[ETH_PTR_MAC_SRC]);

    // Construct UDP protocol header
    // -----------------------------
    // See RFC 768, p. 1
    // Source port
    buffer_out[UDP_PTR_PORT_SRC_H] = buffer_in[UDP_PTR_PORT_DST_H];
    buffer_out[UDP_PTR_PORT_SRC_L] = buffer_in[UDP_PTR_PORT_DST_L];
    // Destination port
    buffer_out[UDP_PTR_PORT_DST_H] = buffer_in[UDP_PTR_PORT_SRC_H];
    buffer_out[UDP_PTR_PORT_DST_L] = buffer_in[UDP_PTR_PORT_SRC_L];
    // Length: ignore, set correctly in updClientSend
    // Checksum: 0, set correctly in udpClientSend
    buffer_out[UDP_PTR_CHECKSUM_H] = 0;
    buffer_out[UDP_PTR_CHECKSUM_L] = 0;

    // Return the pointer to the starting point of the data of the UDP packet
    return &buffer_out[UDP_PTR_DATA];
}

#endif // NET_UDP_SERVER
#endif // NET_UDP

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

uint8_t *add_syn_options() {
    // Get starting index
    uint8_t *buff = &buffer_out[TCP_PTR_OPTIONS];
    // Options:
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
    // Update header length (+ 0x02, 2x 4 bytes)
    buffer_out[TCP_PTR_DATA_OFFSET] += 0x02 << 4;
    // Return new data start location
    return &buffer_out[TCP_PTR_DATA_OPTS];
}

uint8_t *construct(uint16_t src_port, uint8_t *dst_ip, uint16_t dst_port, uint8_t *dst_mac) {
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
    *buff++ = 0x05 << 4;
    // Flags: no flags [TCP_PTR_FLAGS]
    *buff++ = 0;
    // Window: 1024 bytes max (0x400) [TCP_PTR_WINDOW]
    *buff++ = 0x04;
    *buff++ = 0;
    // Checksum: set to 0 [TCP_PTR_CHECKSUM_H]
    *buff++ = 0;
    *buff++ = 0;
    // Urgent pointer: set to 0 [TCP_PTR_URGENT_H]
    *buff++ = 0;
    *buff++ = 0;

    return &buffer_out[TCP_PTR_DATA];
}

uint8_t *tcp_prepare(uint16_t src_port, uint8_t *dst_ip, uint16_t dst_port, uint8_t *dst_mac) {
    construct(src_port, dst_ip, dst_port, dst_mac);
    tcp_add_flags(TCP_FLAG_SYN);
    return add_syn_options();
}

void tcp_send(uint16_t length) {
    uint16_t tmp, len_tcp;

    // TCP packet length
    len_tcp = (buffer_out[TCP_PTR_DATA_OFFSET] >> 4) * 4;

    // IP packet length
    tmp = IP_LEN_HEADER + len_tcp + length;
    buffer_out[IP_PTR_LENGTH_H] = tmp >> 8;
    buffer_out[IP_PTR_LENGTH_L] = tmp & 0xFF;

    // Calculate checksum IP header
    tmp = checksum(&buffer_out[IP_PTR], IP_LEN_HEADER, CHK_IP);
    buffer_out[IP_PTR_CHECKSUM_H] = tmp >> 8;
    buffer_out[IP_PTR_CHECKSUM_L] = tmp & 0xFF;

    // Calculate checksum TCP header
    tmp = checksum(&buffer_out[IP_PTR_SRC], 8+len_tcp+length, CHK_TCP);
    buffer_out[TCP_PTR_CHECKSUM_H] = tmp >> 8;
    buffer_out[TCP_PTR_CHECKSUM_L] = tmp & 0xFF;

    tmp = ETH_LEN_HEADER + IP_LEN_HEADER + len_tcp + length;
#ifdef UTILS_WERKTI_MORE
    werkti_tcp_out += tmp;
#endif // UTILS_WERKTI_MORE

    // Send packet to chip
    network_send(tmp);
}

// Port services list
#ifdef NET_TCP_SERVER
// Check if port list size is defined
#ifndef NET_TCP_SERVICES_LIST_SIZE
#error NET_TCP_SERVICES_LIST_SIZE not defined, but NET_TCP_SERVER active
#endif // NET_TCP_SERVICES_LIST_SIZE
// Create port service list
port_service_t port_services[NET_TCP_SERVICES_LIST_SIZE];

void tcp_server_init(void) {
    // Prepare port list
    port_service_init(port_services, NET_TCP_SERVICES_LIST_SIZE);
}

void tcp_receive(void) {
    #ifdef UTILS_WERKTI_MORE
    // Update werkti udp in
    werkti_tcp_in += buffer_in_length;
    #endif // UTILS_WERKTI_MORE

    // Notify TCP type
    debug_string_p(PSTR("TCP: "));
    // Get type
    uint8_t type = buffer_in[TCP_PTR_FLAGS];
    // Check if it is a reset request
    if (type & TCP_FLAG_RESET) {
        debug_string_p(PSTR("RST "));
        // Reply request
        debug_string_p(PSTR("TODO"));
        debug_newline();
        // Notify finish
        //debug_ok();
        // Do not process request further
    }
    // Check if it is a syn request
    else if (type & TCP_FLAG_SYN) {
        debug_string_p(PSTR("SYN "));
        // Prepare reply
        tcp_prepare_reply();
        // Add options
        add_syn_options();
        // Increase seq with sequence_nr
        add_value_to_buffer(sequence_nr++, &buffer_out[TCP_PTR_SEQ_NR], 4);
        // Increase ack with 1
        add_value_to_buffer(1, &buffer_out[TCP_PTR_ACK_NR], 4);
        // Set syn and ack flag
        tcp_add_flags(TCP_FLAG_SYN | TCP_FLAG_ACK);
        // Send packet
        tcp_send(0);
        // Notify finish
        debug_ok();
    }
    // Check if it is a fin request
    else if (type & TCP_FLAG_FIN) {
        debug_string_p(PSTR("FIN "));
        // Prepare reply
        tcp_prepare_reply();
        // Increase ack with 1
        add_value_to_buffer(1, &buffer_out[TCP_PTR_ACK_NR], 4);
        // Set fin and ack flag
        tcp_add_flags(TCP_FLAG_FIN | TCP_FLAG_ACK);
        // Send packet
        tcp_send(0);
        // Notify finish
        debug_ok();
    }
    // Check if it a single ack, we do not have to process it
    else if (type == TCP_FLAG_ACK) {
        debug_string_p(PSTR("ACK"));
        debug_ok();
    }
    // Check if it is a push request
    else if (type & TCP_FLAG_PUSH) {
        debug_string_p(PSTR("PSH "));

        // Prepare variables
        uint16_t port;
        void (*callback)(uint8_t *data, uint16_t length);

        // Retrieve port
        port  = ((uint16_t)buffer_in[TCP_PTR_PORT_DST_H]) << 8;
        port |= buffer_in[TCP_PTR_PORT_DST_L];
        debug_number(port);
        debug_string_p(PSTR(" "));

        // Retrieve length of packet
        uint16_t pkt_length = ((uint16_t)buffer_in[IP_PTR_LENGTH_H]) << 8;
        pkt_length |= buffer_in[IP_PTR_LENGTH_L];
        // Substract IP header length
        pkt_length -= IP_LEN_HEADER;
        // Substract TCP header length
        pkt_length -= (buffer_in[TCP_PTR_DATA_OFFSET] >> 4) * 4;
        debug_number_as_hex(pkt_length);

        // ACK packet
        tcp_prepare_reply();
        // Increase ack with length
        add_value_to_buffer(pkt_length, &buffer_out[TCP_PTR_ACK_NR], 4);
        // Set ACK flag
        tcp_add_flags(TCP_FLAG_ACK);
        // Send packet
        tcp_send(0);
        debug_string_p(PSTR(" ack "));

        // Check if a listener is registered for this port
        // Add space in front because of number debug before
        debug_string_p(PSTR("service "));
        callback = port_service_get(port_services, NET_TCP_SERVICES_LIST_SIZE, port);
        if (callback) {
            debug_ok();
            // Call callback function
            callback(&buffer_in[buffer_in_length-pkt_length], pkt_length); // Execute callback
        } else {
            // Notify error
            debug_error();
        }
    }
    // Other cases
    else {
        // Notify type
        debug_string_p(PSTR("UNKNOWN: "));
        debug_number_as_hex(type);
        debug_newline();
    }
}

void tcp_port_register(uint16_t port, void (*callback)(uint8_t *data, uint16_t length)) {
    port_service_set(port_services, NET_TCP_SERVICES_LIST_SIZE, port, callback);
}

void tcp_port_unregister(uint16_t port) {
    port_service_remove(port_services, NET_TCP_SERVICES_LIST_SIZE, port);
}

uint8_t *tcp_prepare_reply(void) {
    // Prepare using construct method
    construct(
      // Source port
      (buffer_in[TCP_PTR_PORT_DST_H] << 8) | buffer_in[TCP_PTR_PORT_DST_L],
      // Destination ip
      &buffer_in[IP_PTR_SRC],
      // Destination port
      (buffer_in[TCP_PTR_PORT_SRC_H] << 8) | buffer_in[TCP_PTR_PORT_SRC_L],
      // Destination mac
      &buffer_in[ETH_PTR_MAC_SRC]
    );

    // Switch seq and ack
    uint8_t i;
    for(i = 0; i < 4; i++) {
        buffer_out[TCP_PTR_SEQ_NR+i] = buffer_in[TCP_PTR_ACK_NR+i];
    }
    for(i=0; i<4; i++) {
        buffer_out[TCP_PTR_ACK_NR+i] = buffer_in[TCP_PTR_SEQ_NR+i];
    }

    return &buffer_out[TCP_PTR_DATA];
}

#endif // NET_TCP_SERVER
#endif // NET_TCP

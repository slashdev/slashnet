/**
 * @file werkti.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.10.0
 */

#include "werkti.h"

#if !defined(UTILS_WERKTI) && defined(UTILS_WERKTI_MORE)
#error UTILS_WERKTI_MORE cannot work without UTILS_WERKTI
#endif

// Do we want werkti?
#if defined(UTILS_WERKTI)

// Check if UDP is enabled
#ifndef NET_UDP
#error Werkti cannot work without NET_UDP
#endif // NET_UDP

// Only build if requirements are met
#if defined(NET_UDP)

// Defines
// --------------------------------------------------------------------
#define WERKTI_TYPE_ALL  1
#define WERKTI_TYPE_ARP  2
#define WERKTI_TYPE_ICMP 3
#define WERKTI_TYPE_UDP  4
#define WERKTI_TYPE_TCP  5

// Variables
// --------------------------------------------------------------------

uint8_t  werkti_remote_mac[6] = { WERKTI_REMOTE_MAC };
uint8_t  werkti_remote_ip[4] = { WERKTI_REMOTE_IP };
uint16_t werkti_in;
uint16_t werkti_out;
volatile uint16_t time;
#ifdef UTILS_WERKTI_MORE
uint16_t werkti_arp_in;
uint16_t werkti_arp_out;
uint16_t werkti_icmp_in;
uint16_t werkti_icmp_out;
uint16_t werkti_udp_in;
uint16_t werkti_udp_out;
uint16_t werkti_tcp_in;
uint16_t werkti_tcp_out;
#endif // UTILS_WERKTI_MORE

// Functions
// --------------------------------------------------------------------

void werkti_tick(void) {
    time++;
}

void send_report(uint8_t type, uint16_t in, uint16_t out);

void werkti_maybe_report(void) {
    if (time >= UTILS_WERKTI_REPORT_INTERVAL) {
        debug_string_p(PSTR("WERKTI: IN: "));
        debug_number(werkti_in);
        debug_string_p(PSTR(", OUT: "));
        debug_number(werkti_out);
        debug_newline();
        // Save overal out to temporary variable as it changes during report sending
        uint16_t out = werkti_out;
        werkti_out = 0;
#ifdef UTILS_WERKTI_MORE
        // Same for UDP
        uint16_t udp_out = werkti_udp_out;
        werkti_udp_out = 0;
#endif

        // Overal
        // Send report
        send_report(WERKTI_TYPE_ALL, werkti_in, out);
        // Reset variable, werkti_out already reset
        werkti_in = 0;

#ifdef UTILS_WERKTI_MORE
        // ARP
        // Send report
        send_report(WERKTI_TYPE_ARP, werkti_arp_in, werkti_arp_out);
        // Reset variables
        werkti_arp_in = 0;
        werkti_arp_out = 0;

        // ICMP
        // Send report
        send_report(WERKTI_TYPE_ICMP, werkti_icmp_in, werkti_icmp_out);
        // Reset variables
        werkti_icmp_in = 0;
        werkti_icmp_out = 0;

        // UDP
        // Send report
        send_report(WERKTI_TYPE_UDP, werkti_udp_in, udp_out);
        // Reset variable, werktiUdpOut already reset
        werkti_udp_in = 0;

        // TCP
        // Send report
        send_report(WERKTI_TYPE_TCP, werkti_tcp_in, werkti_tcp_out);
        // Reset variables
        werkti_tcp_in = 0;
        werkti_tcp_out = 0;

#endif // UTILS_WERKTI_MORE

        // Debug: output bytes received and send
        debug_string_p(PSTR("WERKTI: report send\r\n"));
        // Update time
        time = 0;
    }
}

void send_report(uint8_t type, uint16_t in, uint16_t out) {
    uint8_t i = 0;

    // Create UDP header
    uint8_t *buf = udp_prepare(0, werkti_remote_ip, WERKTI_REMOTE_PORT, werkti_remote_mac);

    // Add MAC address
    while (i < 6) {
        buf[i] = my_mac[i];
        i++;
    }

    // Add type of message
    buf[6] = type;

    // Add in and out
    buf[7] = in >> 8;
    buf[8] = in & 0xFF;
    buf[9] = out >> 8;
    buf[10] = out & 0xFF;

    udp_send(11);
}

#endif // NET_UDP
#endif // UTILS_WERKTI

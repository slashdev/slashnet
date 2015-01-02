/**
 * @file dhcp.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.9.0
 */

#include "dhcp.h"

// Do we want DHCP?
#ifdef NET_DHCP

// Check if UDP is enabled
#ifndef NET_UDP
#error DHCP cannot work without NET_UDP
#endif // NET_UDP

// Only build if requirements are met
#if defined(NET_UDP)

// Defines
// -------

// DHCP packet types
#define DHCP_DISCOVER 1 // Client to server
#define DHCP_OFFER    2 // Server to client
#define DHCP_REQUEST  3 // Client to server
#define DHCP_DECLINE  4 // Client to server
#define DHCP_ACK      5 // Server to client
#define DHCP_NACK     6 // Server to client
#define DHCP_RELEASE  7 // Client to server

// DHCP ports
#define DHCP_PORT_SRC 67
#define DHCP_PORT_DST 68

// Pointer to the start of DHCP options
// Relative to UDP_PTR_DATA
// See RFC 2131, p. 10
#define DHCP_PTR_OPTIONS 240

#define DHCP_OPT_SUBNET            1
#define DHCP_OPT_ROUTER            3
#define DHCP_OPT_HOSTNAME         12
#define DHCP_OPT_REQUESTEDIP      50
#define DHCP_OPT_LEASETIME        51
#define DHCP_OPT_TYPE             53
#define DHCP_OPT_SERVERIDENTIFIER 54
#define DHCP_OPT_PARAMETERREQUEST 55

// Variables
// ---------

// Current second
volatile uint8_t dhcp_seconds;
// Has a discover been send?
volatile uint8_t discover_sent;
// Unique DHCP transaction id
volatile uint8_t transaction_id;

// Lease time of an IP address
volatile uint16_t lease_time;
// Server identifier (IP address)
volatile uint8_t server_identifier[4] = { 0x00, 0x00, 0x00, 0x00 };

// Functions
// ---------
void    send_discover(void);
void    send_request(void);
void    prepare(void);
uint8_t is_packet_for_me(void);
uint8_t is_transaction_id(void);
uint8_t get_packet_type(void);
void    parse_ip_address(void);
void    parse_options(void);

// Initial IP request
// Returns 1 on valid IP, don't call the function again when a valid IP is received
uint8_t  dhcp_request_ip(void) {
    if (!network_is_link_up())
        return (0);

    // No packet received
    if (buffer_in_length == 0) {
        // Delay the startup for 3 seconds
        if (dhcp_seconds < 3)
            return (0);

        // Send DHCP discover
        if (!discover_sent) {
            discover_sent = 1;
            dhcp_seconds = 3;
            transaction_id = my_mac[5];
            // Enable broadcast to send ARP DHCP packet
            network_broadcast_enable();
            debug_string_p(PSTR("DHCP: Send discover..."));
            send_discover();
            debug_string_p(PSTR("sent\r\n"));
            return (0);
        }

        // After 30 seconds, no IP address received
        // Resend discover, now with different unique id
        if (my_ip[0] == 0 && dhcp_seconds > 33) {
            transaction_id++;
            dhcp_seconds = 3;
            // Make sure broadcast is enabled
            network_broadcast_enable();
            debug_string_p(PSTR("DHCP: Resend discover..."));
            send_discover();
            debug_string_p(PSTR("sent\r\n"));
            return (0);
        }
    }

    // Check if the packet is for me
    if (is_packet_for_me()) {
#ifdef UTILS_WERKTI_MORE
        // Update werkti udp in
        werkti_udp_in += buffer_in_length;
#endif // UTILS_WERKTI_MORE

        debug_string_p(PSTR("DHCP: Received DHCP packet\r\n"));
        // When the device is power cycled during DHCP request,
        // this case could happen
        if (is_transaction_id())
            return (0); // Should have been initial transaction id, return

        // Get the packet type
        uint8_t type = get_packet_type();

        // DHCP_OFFER packet
        if (type == DHCP_OFFER) {
            debug_string_p(PSTR("DHCP: Type: Offer\r\n"));
            discover_sent = 1;
            parse_ip_address();
            parse_options();
            // Answer offer with a request
            debug_string_p(PSTR("DHCP: Send request..."));
            send_request();
            debug_string_p(PSTR("sent\r\n"));
            return (0);
        }

        // DHCP_ACK packet
        if (type == DHCP_ACK) {
            debug_string_p(PSTR("DHCP: Type: Ack\r\n"));
            // Success!
            // We got the IP address
            discover_sent = 1;
            // Disable broadcast of packets
            network_broadcast_disable();
            return (1);
        }
    }

    // No luck
    return (0);
}

// The discover packet as described in RFC 2131
void send_discover(void) {
    // Create DHCP packet template
    prepare();

    // Packet type
    // See RFC 1533, p. 24, chap. 9.4
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS]     = DHCP_OPT_TYPE;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 1] = 1;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 2] = DHCP_DISCOVER;

    // Parameter request list
    // See RFC 1533, p. 25, chap. 9.6
    // We want: Subnet mask (1), Router address (gateway address, 3)
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3] = DHCP_OPT_PARAMETERREQUEST;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 4] = 2;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 5] = DHCP_OPT_SUBNET;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 6] = DHCP_OPT_ROUTER;

    // Do we need to send a hostname?
    uint8_t i = 0;
#ifdef NET_DHCP_HOSTNAME
    // Hostname
    // See RFC 1533, p. 8, chap 3.14
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 7] = DHCP_OPT_HOSTNAME;
    char *hostname = NET_DHCP_HOSTNAME;
    while (*hostname) {
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 9 + i] = *hostname++;
        i++;
    }
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 8] = i;
    i += 2;
#endif

    // End of options
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 7 + i] = 0xFF;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 8 + i] = 0;

    // Send buffer by UDP
    // Length of UDP data: DHCP_PTR_OPTIONS + 8 + i
    udp_send(DHCP_PTR_OPTIONS + 8 + i);
}

// The request packet as described in RFC 2131
void send_request(void) {
    uint8_t i = 0;
    uint8_t j = 0;

    // Create DHCP packet template
    prepare();

    // Packet type
    // See RFC 1533, p. 24, chap. 9.4
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS]     = DHCP_OPT_TYPE;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 1] = 1;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 2] = DHCP_REQUEST;
    i = 3;

    // Server identifier
    // See RFC 1533, p. 24, chap. 9.5
    if (server_identifier[0] != 0) {
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i]     = DHCP_OPT_SERVERIDENTIFIER;
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 1] = 4;
        j = 0;
        while(j < 4) {
            buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 2 + j] = server_identifier[j];
            j++;
        }
        i += 6;
    }

    // Requested IP address
    // See RFC 1533, p. 23, chap. 9.1
    if (my_ip[0] != 0) {
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i]     = DHCP_OPT_REQUESTEDIP;
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 1] = 4;
        j = 0;
        while(j < 4) {
            buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 2 + j] = my_ip[j];
            j++;
        }
        i += 6;
    }

    // Parameter request list
    // See RFC 1533, p. 25, chap. 9.6
    // We want: Subnet mask (1), Router address (gateway address, 3)
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i]     = DHCP_OPT_PARAMETERREQUEST;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 1] = 2;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 2] = DHCP_OPT_SUBNET;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 3] = DHCP_OPT_ROUTER;

#ifdef NET_DHCP_HOSTNAME
    // Hostname
    // See RFC 1533, p. 8, chap 3.14
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 4] = DHCP_OPT_HOSTNAME;
    char *hostname = NET_DHCP_HOSTNAME;
    j = 0;
    while (*hostname) {
        buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 6 + j] = *hostname++;
        j++;
    }
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 5] = j;
    i += j + 2;
#endif // NET_DHCP_HOSTNAME

    // End of options
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 4] = 0xFF;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + i + 5] = 0;

    // Send buffer by UDP
    // Length of UDP data: DHCP_PTR_OPTIONS + i + 5
    udp_send( DHCP_PTR_OPTIONS + i + 5);
}

// Create a template for DHCP packets
// All fields except options are set.
// Mostly filled with zeros.
// See RFC 2131, p. 9
void prepare(void) {
    uint8_t i = 0;

    // Broadcast mac, used for IP as well
    uint8_t all_FF[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    // Let UDP client create a template for the packet
    udp_prepare(DHCP_PORT_DST, all_FF, DHCP_PORT_SRC, all_FF);

    // Source IP is 0.0.0.0
    while (i<4) {
        buffer_out[IP_PTR_SRC + i] = 0;
        i++;
    }

    // Fill the bootstrap protocol starting at UDP_PTR_DATA
    // Message type = boot request
    buffer_out[UDP_PTR_DATA] = 1;
    // Hardware address type, 1 = 10 Mbit
    buffer_out[UDP_PTR_DATA + 1] = 1;
    // Hardware address length (mac address)
    buffer_out[UDP_PTR_DATA + 2] = 6;
    // Hops needs to be set to 0
    buffer_out[UDP_PTR_DATA + 3] = 0;
    // We use a single byte transaction id, and we have 4 bytes to use.
    // The first byte will be used to distinguish initial from renew requests.
    // When the first byte is 1, it is an initial request, else renew.
    buffer_out[UDP_PTR_DATA + 4] = 1;
    buffer_out[UDP_PTR_DATA + 5] = transaction_id;
    buffer_out[UDP_PTR_DATA + 6] = transaction_id;
    buffer_out[UDP_PTR_DATA + 7] = transaction_id;
    // Seconds (2b), Flags (2b), Client address (4b), your address (4b),
    // next server (4b) and relay agent (4b) are set to zeros
    i = 8;
    while (i < 28) {
        buffer_out[UDP_PTR_DATA + i] = 0;
        i++;
    }
    // Client hardware address (mac address)
    i = 0;
    while (i < 6) {
        buffer_out[UDP_PTR_DATA + 28 + i] = my_mac[i];
        i++;
    }
    // Remaining 10 bytes of client hardware address filled with zeros.
    // Server host name and boot file name as zeros.
    i = 34;
    while (i < 236) {
        buffer_out[UDP_PTR_DATA + i] = 0;
        i++;
    }
    // Magic cookie
    // See RFC 2131, p. 13, chap. 3
    buffer_out[UDP_PTR_DATA + 236] = 99;
    buffer_out[UDP_PTR_DATA + 237] = 130;
    buffer_out[UDP_PTR_DATA + 238] = 83;
    buffer_out[UDP_PTR_DATA + 239] = 99;
}

// Check if a packet is meant for us
uint8_t is_packet_for_me(void) {
    // Smallest packet would be with empty options, so length has to be
    // at least UDP_PTR_DATA + DHCP_PTR_OPTIONS
    if (buffer_in_length < (UDP_PTR_DATA + DHCP_PTR_OPTIONS))
        return (0);
    // Is the packet coming from the right port?
    // Should be DHCP_PORT_SRC
    if (buffer_in[UDP_PTR_PORT_SRC_L] != DHCP_PORT_SRC)
        return (0);
    // First byte shows packet type:
    // 1 BOOTREQUEST
    // 2 BOOTREPLY (we want this one)
    if (buffer_in[UDP_PTR_DATA] != 2)
        return (0);
    // Does the transaction id match?
    // Check second to fourth byte
    if (buffer_in[UDP_PTR_DATA + 5] != transaction_id
        || buffer_in[UDP_PTR_DATA + 6] != transaction_id
        || buffer_in[UDP_PTR_DATA + 7] != transaction_id)
        return (0);
    // We can be sure it's for us now
    return (1);
}

// Check is this packet is part of a renew
uint8_t is_transaction_id(void) {
    // Smallest packet would be with empty options, so length has to be
    // at least UDP_PTR_DATA + DHCP_PTR_OPTIONS
    if (buffer_in_length < (UDP_PTR_DATA + DHCP_PTR_OPTIONS))
        return (0);
    // If the first byte of the transaction id field is 2, it is a renew
    // request. If it is 1 it's an initial request.
    if (buffer_in[UDP_PTR_DATA + 4] == 2)
        return (1);
    return (0);
}

// Parse the option fields for DHCP packet type option. If found return its
// value, else return 0. The values can be matched against the list of DHCP
// packet types in the header file.
// See RFC 1533, p. 24, chap. 9.4
uint8_t get_packet_type(void) {
    uint16_t index = 0;
    uint8_t length = 0;

    // Smallest option is 3 bytes, so length has to be at least
    // UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3
    if (buffer_in_length < (UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3))
        return (0);

    // Options are all coded in the form:
    // Type (1b), length (1b), value (length b)
    index = UDP_PTR_DATA + DHCP_PTR_OPTIONS;
    while ((index + 2) < buffer_in_length) {
        length = buffer_in[index + 1];
        if ((length < 1) || ((index + length + 1) > buffer_in_length))
            break;
        if (buffer_in[index] == DHCP_OPT_TYPE)
            return (buffer_in[index + 2]);
        index += 2 + length;
    }
    return (0);
}

// This is used to read your IP address from a DHCP_ACK or DHCP_OFFER packet
// See RFC 2131, p. 10
void parse_ip_address(void) {
    // Packet should be at least of length UDP_PTR_DATA + 20
    // As at UDP_PTR_DATA + 16 your address starts and has a length of 4 bytes
    if (buffer_in_length < (UDP_PTR_DATA + 20))
        return;
    if (buffer_in[UDP_PTR_DATA + 16] != 0) {
        // We received a your address
        uint8_t i = 0;
        while(i < 4) {
            my_ip[i] = buffer_in[UDP_PTR_DATA + 16 + i];
            i++;
        }
    }
}

// Following RFC 1533, options will be parsed from the packet and settings
// updated where needed.
void parse_options(void) {
    uint16_t index;
    uint8_t length;
    uint8_t i;

    // Smallest option is 3 bytes, so length has to be at least
    // UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3
    if (buffer_in_length < (UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3))
        return;

    // Options are all coded in the form:
    // Type (1b), length (1b), value (length b)
    index = UDP_PTR_DATA + DHCP_PTR_OPTIONS;
    while ((index + 2) < buffer_in_length) {
        length = buffer_in[index + 1];
        if ((length < 1) || ((index + length + 1) > buffer_in_length))
            break;
        switch (buffer_in[index]) {

                // Padding, we should not read those
                // Stop the loop
            case 0:
                index = buffer_in_length;
                break;

                // Subnet mask
                // See RFC 1533, p. 4, chap. 3.3
            case DHCP_OPT_SUBNET:
                if (length == 4) {
                    i = 0;
                    while (i < 4) {
                        gateway_netmask[i] = buffer_in[index + 2 + i];
                        i++;
                    }
                }
                break;

                // Router, we call it gateway
                // See RFC 1533, p. 5, chap. 3.5
            case DHCP_OPT_ROUTER:
                if (length == 4) {
                    i = 0;
                    while (i < 4) {
                        gateway_ip[i] = buffer_in[index + 2 + i];
                        i++;
                    }
                }
                break;

                // Lease time
                // See RFC 1533, p. 23, chap. 9.2
                // The lease time is an 32 bit value telling you how many seconds
                // your lease time is. A value of 0xFFFFFFFF represents infinity.
                // Anything less is converted to minutes by >> 6 (divides by 64,
                // almost a minute).
            case DHCP_OPT_LEASETIME:
                if (length != 4) {
                    // Take a reasonable value, 1/3rd of a day.
                    lease_time = 480;
                    break;
                }

                i = 0;
                uint32_t lease = 0;
                while (i < 4) {
                    lease = (lease << 8) | buffer_in[index + 2 + i];
                    i++;
                }

                // Is the lease infinite?
                if (lease == 0xFFFFFFFF) {
                    lease_time = 0xFFFF;
                    break;
                }

                // Right shift by 6 to divide by 64. This is almost like
                // dividing by 60, so a decent approach to minutes.
                lease = lease >> 6;

                // Is the lease time larger than our representation of
                // infinity? If so, make it 0xFFFD so we release it.
                if (lease > 0xFFFD) {
                    lease_time = 0xFFFD;
                } else {
                    lease_time = lease & 0xFFFF;
                }

                // Output new lease time
                debug_string_p(PSTR("DHCP: New lease time: "));
                debug_number(lease_time);
                debug_newline();

                // We need at least 5 minutes
                if (lease_time < 5)
                    lease_time = 5;
                break;

                // RFC 2131: A DHCP server always returns it own address in the
                // server identifier option
            case DHCP_OPT_SERVERIDENTIFIER:
                if (length == 4) {
                    i = 0;
                    while (i < 4) {
                        server_identifier[i] = buffer_in[index + 2 + i];
                        i++;
                    }
                }
                break;
        }
        index += 2 + length;
    }
}

#ifndef NET_DHCP_NO_RENEWAL

void send_renew(void);

// IP lease time renewal
// Returns packet length after usage (or not)
uint16_t dhcp_renew(void) {
    // Is it time to renew the lease?
    if (dhcp_seconds > 50) {
        // Slightly faster than a minute, but it's better to
        // ask a bit too early than too late
        dhcp_seconds = 0;
        // Decrease the lease time, unless it's infinite
        if (lease_time < 0xFFFF && lease_time > 1) {
            lease_time--;
        }
    }

    // Check if we need to send a renew packet
    if (buffer_in_length == 0 && lease_time < 3) {
        // Do we have a link?
        // If not, return the packet
        if (!network_is_link_up())
            return (buffer_in_length);
        // Make a new unique transaction id
        transaction_id++;
        // Send the renew packet
        debug_string_p(PSTR("DHCP: Send renew..."));
        send_renew();
        debug_string_p(PSTR("sent\r\n"));
        // Set the lease time two minutes from now
        // If no answer is given, a retry is performed
        lease_time = 5;
        return (0);
    }

    // Is the packet for me?
    if (buffer_in_length && is_packet_for_me()) {
#ifdef UTILS_WERKTI_MORE
        // Update werkti udp in
        werkti_udp_in += buffer_in_length;
#endif // UTILS_WERKTI_MORE

        debug_string_p(PSTR("DHCP: Received renew packet\r\n"));
        // Is it an DHCP_ACK packet?
        if (get_packet_type() == DHCP_ACK) {
            debug_string_p(PSTR("DHCP: Type: Ack (renew)\r\n"));
            // Check if this packet is part of a renew
            if (is_transaction_id()) {
                // Get information from packet
                // New lease time
                debug_string_p(PSTR("DHCP: Received new lease time\r\n"));
                parse_options();
                // Packet handled, reset buffer length
                buffer_in_length = 0;
            }
        }
        return (0);
    }

    // Hand the packet over as is
    return (buffer_in_length);
}

// The renew packet as described in RFC 2131
// Chapter 4.3.6 (p. 33) says server identifier and requested IP address
// should not be filled, but zeros. Also it says that ciaddr (client IP
// address) should be filled with the requested IP address.
// This same chapter suggest this packet can be send as unicast, but tests
// show not all DHCP servers listen to unicast. Therefor we send it broadcast.
// However, we do expect an unicast answer.
void send_renew(void) {
    uint8_t i = 0;
    // Create DHCP packet template
    prepare();

    // First byte of transaction id is set to 2, this identifies a renew
    // request. This makes processing an DHCP_ACK easier.
    buffer_out[UDP_PTR_DATA + 4] = 2;

    // Set the source IP address in the IP header
    i = 0;
    while (i < 4) {
        buffer_out[IP_PTR_SRC + i] = my_ip[i];
        i++;
    }

    // RFC 2131, p. 33, chap. 4.3.6
    // Server identifier and requested IP address should be zeros.
    // The templates zeros these by default.
    // ciaddr should be filled with the requested IP address.
    i = 0;
    while (i < 4) {
        buffer_out[UDP_PTR_DATA + 12 + i] = my_ip[i];
        i++;
    }

    // Packet type
    // See RFC 1533, p. 24, chap. 9.4
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS]     = DHCP_OPT_TYPE;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 1] = 1;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 2] = DHCP_REQUEST;

    // End of options
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 3] = 0xFF;
    buffer_out[UDP_PTR_DATA + DHCP_PTR_OPTIONS + 4] = 0;

    // Send buffer by UDP
    // Length of UDP data: DHCP_PTR_OPTIONS + 4
    udp_send(DHCP_PTR_OPTIONS + 4);
}

#endif // NET_DHCP_NO_RENEWAL
#endif // NET_UDP
#endif // NET_DHCP

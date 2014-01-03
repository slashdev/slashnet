/**
 * @file config.h
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#include "config.h"

// MAC address of the device. Needs to be unique in the network.
// Only change last two digits and make sure every device in a network has
// a unique mac address!
uint8_t my_mac[6] = { 0x2E, 0x44, 0x65, 0x76,   0x00, 0x01 };

// IPv4 address of the device. Needs to be set when not using DHCP.
// When using DHCP it is set automatically.
uint8_t my_ip[4]  = { 0x00, 0x00, 0x00, 0x00 };

// Mac and IP address of the gateway, net mask
// When using DHCP you do not need to these, they will be provided
// Without DHCP you need to set these
uint8_t gateway_mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t gateway_ip[4]  = { 0x00, 0x00, 0x00, 0x00 };
uint8_t gateway_netmask[4]  = { 0x00, 0x00, 0x00, 0x00 };

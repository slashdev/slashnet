/**
 * @file network.h
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.3.0
 */

#include "network.h"

// Do we want network?
#ifdef NET_NETWORK

// Check if F_CPU is set
#ifndef F_CPU
#error F_CPU is not set but required for network
#endif // F_CPU

// Defines
// ---------

// Port and data direction register
#define NETWORK_PORT PORTB
#define NETWORK_DDR  DDRB

// Control pins
#define NETWORK_CTR_CS  PORTB4
#define NETWORK_CTR_SI  PORTB5
#define NETWORK_CTR_SO  PORTB6
#define NETWORK_CTR_SCK PORTB7

// Enable or disable SPI selector
#define NETWORK_SPI_ACTIVE() SPI_ACTIVE(NETWORK_PORT, NETWORK_CTR_CS)
#define NETWORK_SPI_PASSIVE() SPI_PASSIVE(NETWORK_PORT, NETWORK_CTR_CS)

// Variables
// ---------

// External addresses from config
extern uint8_t my_ip[];
extern uint8_t my_mac[];

// Current register
static uint8_t bank;
// Next packet pointer
static uint16_t next_packet_ptr;

// Buffer for recieved packets
uint8_t buffer_in[BUFFER_IN_SIZE+1];
// Buffer for packets to transmit
uint8_t buffer_out[BUFFER_OUT_SIZE];
// Length of received packet
uint16_t buffer_in_length;

#endif // NET_NETWORK

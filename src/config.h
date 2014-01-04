/**
 * @file config.h
 * @brief Configuration of EthShield
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <inttypes.h>

/**
 * @brief Version of the EthShield firmware
 */
#define VERSION "0.5.0"

/**
 * Clock setting
 */
#ifndef F_CPU
#define F_CPU 20000000
#endif


/**********************************************************************
 * Communication channels
 **********************************************************************/

//
// SPI
// --------------------------------------------------------------------

/**
 * @brief Enables Serial Peripheral Interface channel
 */
#define COM_SPI

//
// USART
// --------------------------------------------------------------------

/**
 * @brief Enable Universal Synchronous and Asymchronous serial Receiver and
 * Transmitter channel
 */
#define COM_USART

/**
 * @brief USART buffer ring size
 */
#define COM_USART_BUFFER_RING_SIZE 64


/**********************************************************************
 * Network
 **********************************************************************/

//
// Network
// --------------------------------------------------------------------

/**
 * @brief Enable network
 */
#define NET_NETWORK
/**
 * @brief Network buffer in size
 */
#define BUFFER_IN_SIZE 1023
/**
 * @brief Network buffer out size
 */
#define BUFFER_OUT_SIZE 1023


/**********************************************************************
 * Utilities
 **********************************************************************/

//
// Counter
// --------------------------------------------------------------------

/**
 * @brief Enable second counter
 */
#define UTILS_COUNTER

/**
 * @brief Timer selection for counter
 *
 * Choose which counter is used to count seconds. This counter can be used by
 * dhcp to keep track of lease time and to resend messages when a reply
 * did not come in time
 */
//#define UTILS_COUNTER_TIMER0
#define UTILS_COUNTER_TIMER1
//#define UTILS_COUNTER_TIMER2

//
// Debug
// --------------------------------------------------------------------

/**
 * @brief Enable debug over USART
 */
#define UTILS_DEBUG

//
// Uptime
// --------------------------------------------------------------------

/**
 * @brief Enable uptime
 */
#define UTILS_UPTIME

/**********************************************************************
 * DO NOT CHANGE BELOW
 * References from config.c, change them in config.c
 **********************************************************************/

/**
 * @brief MAC address of the device. Needs to be unique in the network.
 * @note Only change last two digits and make sure every device in a network has
 * a unique mac address!
 */
extern uint8_t my_mac[];
/**
 * @brief IPv4 address of the device. Needs to be set when not using DHCP.
 * When using DHCP it is set automatically.
 */
extern uint8_t my_ip[];
/**
 * @brief MAC address of the gateway. Needs to be set when not using DHCP.
 * When using DHCP it is set automatically.
 */
extern uint8_t gateway_mac[];
/**
 * @brief IPv4 address of the gateway. Needs to be set when not using DHCP.
 * When using DHCP it is set automatically.
 */
extern uint8_t gateway_ip[];
/**
 * @brief IPv4 netmask for the network. Needs to be set when not using DHCP.
 * When using DHCP it is set automatically.
 */
extern uint8_t gateway_netmask[];

#endif // CONFIG_H

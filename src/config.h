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
#define VERSION "0.12.3"

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
// Two Wire Interface (I2C, TWI)
// --------------------------------------------------------------------

/**
 * @brief Enables Two Wire Interface
 */
#define COM_I2C

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

//
// Adres Resolution Protocol (ARP)
// --------------------------------------------------------------------

/**
 * @brief ARP enable
 */
#define NET_ARP

/**
 * @brief ARP cache size
 */
#define NET_ARP_CACHE_SIZE 10

//
// Dynamic Host Configuration Protocol (DHCP)
// --------------------------------------------------------------------

/**
 * @brief DHCP enable
 */
#define NET_DHCP

/**
 * @brief Do not use DHCP renewal
 */
//#define NET_DHCP_NO_RENEWAL

/**
 * @brief
 */
#define NET_DHCP_HOSTNAME "SDev-devel"

//
// Internet Control Message Protocol (ICMP)
// --------------------------------------------------------------------

/**
 * @brief ICMP enable
 */
#define NET_ICMP

//
// User Datagram Protocol (UDP)
// --------------------------------------------------------------------

/**
 * @brief UDP enable
 */
#define NET_UDP

/**
 * @brief UDP server enable
 */
#define NET_UDP_SERVER

/**
 * @brief UDP services list size
 */
#define NET_UDP_SERVICES_LIST_SIZE 10


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
// Logger
// --------------------------------------------------------------------

/**
 * @brief Enable logging over USART, level INFO
 * This is the less verbose logger. Shows info_* logging.
 */
#define UTILS_LOGGER_INFO

/**
 * @brief Enable logging over USART, level DEBUG
 * This is the more verbose logger. Shows info_* and debug_* logging.
 */
#define UTILS_LOGGER_DEBUG

//
// Uptime
// --------------------------------------------------------------------

/**
 * @brief Enable uptime
 */
#define UTILS_UPTIME

//
// Werkti
// --------------------------------------------------------------------

/**
 * @brief Werkti enable
 */
#define UTILS_WERKTI

/**
 * @brief Make werkti log traffice per protocol
 */
#define UTILS_WERKTI_MORE

/**
 * @brief At this interval reports are send and counters reset
 * Default: 300
 */
#define UTILS_WERKTI_REPORT_INTERVAL 300

/**
 * @brief Werkti remote server mac address
 * @note Define as  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 */
#define WERKTI_REMOTE_MAC 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

/**
 * @brief Werkti remote server ip address
 * @note Define as 0x00, 0x00, 0x00, 0x00
 */
#define WERKTI_REMOTE_IP 0x00, 0x00, 0x00, 0x00

/**
 * @brief Werkti remote port
 */
#define WERKTI_REMOTE_PORT 7900


/**********************************************************************
 * Extensions
 **********************************************************************/

//
// Led driver TLC59116
// --------------------------------------------------------------------

/**
 * @brief Enable TLC59116 support
 */
#define EXT_TLC59116


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

#ifdef API_WERKTI

// Remote addresses are set in config.c
extern uint8_t werktiRemoteMac[];
extern uint8_t werktiRemoteIp[];
#endif

#endif // CONFIG_H

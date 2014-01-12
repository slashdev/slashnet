/**
 * @file werkti.h
 * @brief Werkti tracks and reports traffic
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.10.0
 */

#ifndef UTILS_WERKTI_H
#define UTILS_WERKTI_H

#include "../config.h"

// Do we want werkti?
#if defined(UTILS_WERKTI) || defined(UTILS_WERKTI_MORE)

#include <inttypes.h>
#include "logger.h"
#include "../net/udp.h"

/**
 * @brief Update the uptime with a second
 */
extern void werkti_tick(void);

/**
 * @brief Maybe send a report to the werkti server
 *
 * When the interval has been passed, a report will be send. Else nothing will
 * be done.
 */
extern void werkti_maybe_report(void);

/**
 * @brief Bytes received
 */
extern uint16_t werkti_in;

/**
 * @brief Bytes send
 */
extern uint16_t werkti_out;

#ifdef UTILS_WERKTI_MORE

/**
 * @brief Bytes received via ARP
 */
extern uint16_t werkti_arp_in;

/**
 * @brief Bytes send via ARP
 */
extern uint16_t werkti_arp_out;

/**
 * @brief Bytes received via ICMP
 */
extern uint16_t werkti_icmp_in;

/**
 * @brief Bytes send via ICMP
 */
extern uint16_t werkti_icmp_out;

/**
 * @brief Bytes received via UDP
 */
extern uint16_t werkti_udp_in;

/**
 * @brief Bytes send via UDP
 */
extern uint16_t werkti_udp_out;

/**
 * @brief Bytes received via TCP
 */
extern uint16_t werkti_tcp_in;

/**
 * @brief Bytes send via TCP
 */
extern uint16_t werkti_tcp_out;

#endif // UTILS_WERKTI_MORE
#endif // UTILS_WERKTI || UTILS_WERKTI_MORE
#endif // UTILS_WERKTI_H

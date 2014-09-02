/**
 * @file port_service.h
 * @brief Port services. Framework to register callback functions for a specific
 * port.
 *
 * To use port services, you should follow these steps:
 * 1. Declare an array of type port_service_t (ex.
 * port_service_t my_services[10])
 * 2. Initialize array by calling port_service_init (ex.
 * port_service_init(my_services, 10);
 *
 * Add port services by calling port_service_set and remove services by calling
 * port_service_remove. Get a service by calling port_service_get.
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 1.0
 */

#ifndef UTILS_PORT_SERVICE_H
#define UTILS_PORT_SERVICE_H

#include "../config.h"


// Do we want port services?
#if defined(NET_UDP_SERVER)
// To avoid complicated conditional checks for the source file, define
// UTILS_PORTSERVICE
#ifndef UTILS_PORTSERVICE
#define UTILS_PORTSERVICE
#endif // UTILS_PORTSERVICE
#endif // NET_UDP_SERVER

#ifdef UTILS_PORTSERVICE

#include <inttypes.h>

/**
 * Port service registration item. This holds the port to which the service is
 * registered and the callback function that should be called when an packet
 * arrives on the registered port.
 */
typedef struct {
    /**
     * Port number the service is registered to.<br />
     * When 0, no service is registered.
     */
    uint16_t port;
    /**
     * Callback function of the service registered.<br />
     * The function receives a pointer to the data block of the packet and the
     * length of this data block.
     */
    void (*callback)(uint8_t *data, uint16_t length);
} port_service_t;

/**
 * @brief Initialize an array of portServiceItem_t
 *
 * @param list Array of portServiceItem_t
 * @param size Size of the array
 */
extern void port_service_init(port_service_t *list, uint8_t size);
/**
 * @brief Add a service callback for a port to a list
 *
 * This will search if the port is already registered and overwrite with the
 * new callback if it exists. If it doesn't exist, it searches the first empty
 * spot and saves the port and callback there. If there are not free slots,
 * the function will terminate.
 *
 * @param list Array to save the port and callback in
 * @param size Size of the array
 * @param port Port to register for
 * @param callback Callback service function to register
 */
extern void port_service_set(port_service_t *list, uint8_t size, uint16_t port, void (*callback)(uint8_t *data, uint16_t length));
/**
 * @brief Remove a service callback for a port from a list
 *
 * This will search if the port is registered and clear is from the list if it
 * exists. If it does not exist, nothing will be changed.
 *
 * @param list Array to remove the port and callback from
 * @param size Size of the array
 * @param port Port to find and clear
 */
extern void port_service_remove(port_service_t *list, uint8_t size, uint16_t port);
/**
 * @brief Retrieve the service callback for a port from a list
 *
 * This will search if the port is registered and if it exists return the
 * service callback that belong to the port.
 *
 * @param list Array to get the callback from
 * @param size Size of the array
 * @param port Port to find and retrieve the callback from
 * @return void* Service callback, 0 if not found
 */
extern void (*port_service_get(port_service_t *list, uint8_t size, uint16_t port))(uint8_t *data, uint16_t length);


#endif // UTILS_PORTSERVICE
#endif // UTILS_PORT_SERVICE_H

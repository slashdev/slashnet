/**
 * @file port_service.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 1.0
 */

#include "port_service.h"

// Do we want port services?
#ifdef UTILS_PORTSERVICE

// Prepare a list of ports with callbacks by resetting to default values
void port_service_init(port_service_t *list, uint8_t size) {
    uint8_t i = 0;

    while (i < size) {
        list[i].port = 0;
        list[i].callback = 0;
        i++;
    }
}

// Add a service callback to an existing list
// Note: only a single callback can be set to a port number, registering a
// second callback on a port will overwrite the first callback
void port_service_set(port_service_t *list, uint8_t size, uint16_t port, void (*callback)(uint8_t *data, uint16_t length)) {
    uint8_t i = 0, index_port = 0, index_empty = 0;

    // Search through the array to see if the port is already used and where
    // the first empty spot is.
    while (i < size) {
        // Is the spot empty?
        if (!index_empty && list[i].port == 0) {
            index_empty = i+1; // +1 to spare a variable for 0 index
        }
        // Is it the same port?
        if (!index_port && list[i].port == port) {
            index_port = i+1; // +1 to spare a variable for 0 index
        }
        i++;
    }

    // If we have found the port, overwrite it
    if (index_port) {
        list[index_port-1].callback = callback;
        return; // We're done
    }

    // If we have found an empty spot, write data
    if (index_empty) {
        list[index_empty-1].port = port;
        list[index_empty-1].callback = callback;
        return; // We're done
    }
}

// Remove a service callback from an existing list
void port_service_remove(port_service_t *list, uint8_t size, uint16_t port) {
    uint8_t i = 0;

    while (i < size) {
        if (list[i].port == port) {
            list[i].port = 0;
            list[i].callback = 0;
            return;
        }
        i++;
    }
}

// Get a service callback from an existing list, returns null if the port has
// no registered service callback
void (*port_service_get(port_service_t *list, uint8_t size, uint16_t port))(uint8_t *data, uint16_t length) {
    uint8_t i = 0;

    while (i < size) {
        if (list[i].port == port) {
            return list[i].callback;
        }
        i++;
    }

    return 0;
}

#endif // UTILS_PORTSERVICE

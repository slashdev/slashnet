/**
 * @file spi.h
 * @brief Serial peripheral interface channel functionality
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.2.0
 */

#ifndef COM_SPI_H
#define COM_SPI_H

#include "../config.h"

// Do we want SPI?
#ifdef COM_SPI

#include <inttypes.h>

/**
 * SPI config
 */
typedef struct spi_config {
    
} spi_config_t;

/**
 * @brief Initialize SPI channel
 * @param config Configuration for spi channel
 */
extern void spi_init(spi_config_t *config);

/**
 * @brief Activate slave select for _pin_ on _port_
 */
#define SPI_ACTIVE(port, pin)  (port) &= ~(1 << (pin))
/**
 * @brief Deactivate slave select for _pin_ on _port_
 */
#define SPI_PASSIVE(port, pin) (port) |=  (1 << (pin))
/**
 * @brief Active wait until SPI finished sending/receiving
 */
#define SPI_WAIT() while(!(SPSR&(1<<SPIF)))

#endif // COM_SPI
#endif // COM_SPI_H

//
//  spi.h
//  Ethernet Shield
//
//  Created by EFCM van der Werf on 12/28/13.
//  Copyright (c) 2013 EFCM van der Werf. All rights reserved.
//

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
    
};

/**
 * @brief Initialize SPI channel
 * @param config Configuration for spi channel
 */
extern void spi_init(spi_config *config);

#define SPI_START(port, pin) (port) &= ~(1 << (pin))
#define SPI_STOP(port, pin)  (port) |=  (1 << (pin))

#endif // COM_SPI
#endif // COM_SPI_H

/**
 * @file spi.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.2.0
 */

#include "spi.h"

// Do we want SPI?
#ifdef COM_SPI

// Check if F_CPU is set
#ifndef F_CPU
#error F_CPU is not set but required for SPI
#endif // F_CPU

void spi_init(spi_config_t *config) {
    // Disable spi to set config
    SPCR &= ~(1 << SPE);

    SPSR = 0
    | (((config->clock_rate & 0x04) >> 2) << SPI2X) // Clock rate (SPI2X)
    | 0;
    SPCR = 0
    | ((config->interrupt & 0x01) << SPIE)      // Interrupt
    | ((config->enable & 0x01) << SPE)          // Enable
    | ((config->data_order & 0x01) << DORD)     // Data order
    | ((config->role & 0x01) << MSTR)           // Role master/slave
    | ((config->clock_polarity & 0x01) << CPOL) // Clock polarity
    | ((config->clock_phase & 0x01) << CPHA)    // Clock phase
    | ((config->clock_rate & 0x03) << SPR0)     // Clock rate (SPR1, SPR0)
    | 0;
}

void spi_get_default_config(spi_config_t *config) {
    config->enable = SPI_DISABLE;
    config->interrupt = SPI_INTERRUPT_DISABLE;
    config->data_order = SPI_DATA_ORDER_MOST_SIGNIFICANT_BIT;
    config->role = SPI_ROLE_SLAVE;
    config->clock_polarity = SPI_CLOCK_POLARITY_IDLE_LOW;
    config->clock_phase = SPI_CLOCK_PHASE_SAMPLE_ON_LEADING;
    config->clock_rate = SPI_CLOCK_RATE_FOSC_4;
}

#endif // COM_SPI

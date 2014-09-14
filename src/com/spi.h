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
#include <avr/io.h>

/**
 * SPI config
 */
typedef struct {
    /**
     * @brief Enable spi, one to enable, zero to disable.
     * __Default: 0, disabled__
     */
    uint8_t enable;
    /**
     * @brief Enable spi interrupt, one to enable, zero to disable.
     * __Default: 0, disabled__
     * @note Use defines SPI_INTERRUPT_ENABLE, SPI_INTERRUPT_DISABLE
     */
    uint8_t interrupt;
    /**
     * @brief Data order, one to send least significant bit first, zero to send
     * most significant bit first.
     * __Default: least significant bit__
     * @note Use defines SPI_DATA_ORDER_LEAST_SIGNIFICANT_BIT,
     * SPI_DATA_ORDER_MOST_SIGNIFICANT_BIT
     */
    uint8_t data_order;
    /**
     * @brief Master/slave selector, one for master, zero for slave
     * __Default: 0, slave__
     * @note Use defines SPI_ROLE_MASTER, SPI_ROLE_SLAVE
     */
    uint8_t role;
    /**
     * @brief Set clock polarity, one to make SCK high when idle, zero to make
     * SCK low when idle.
     * __Default: 0, idle low__
     * @note Use defines SPI_CLOCK_POLARITY_IDLE_HIGH,
     * SPI_CLOCK_POLARITY_IDLE_LOW
     */
    uint8_t clock_polarity;
    /**
     * @brief Clock phase, one to sample on trailing edge, zero to sample on
     * leading edge.
     * __Default: 0, sample on leading edge__
     * @note Use defines SPI_CLOCK_PHASE_SAMPLE_ON_TRAILING,
     * SPI_CLOCK_PHASE_SAMPLE_ON_LEADING
     */
    uint8_t clock_phase;
    /**
     * @brief Clock rate selector, value from 0 to 7.
     * __Default: 0, fosc/4
     * @note Use defines SPI_CLOCK_RATE_FOSC_2, SPI_CLOCK_RATE_FOSC_4,
     * SPI_CLOCK_RATE_FOSC_8, SPI_CLOCK_RATE_FOSC_16, SPI_CLOCK_RATE_FOSC_32,
     * SPI_CLOCK_RATE_FOSC_64, SPI_CLOCK_RATE_FOSC_128
     */
    uint8_t clock_rate;
} spi_config_t;

// SPI config values
// Enable
#define SPI_ENABLE 1
#define SPI_DISABLE 0
// Interrup
#define SPI_INTERRUPT_ENABLE 1
#define SPI_INTERRUPT_DISABLE 0
// Data order
#define SPI_DATA_ORDER_LEAST_SIGNIFICANT_BIT 1
#define SPI_DATA_ORDER_MOST_SIGNIFICANT_BIT 0
// Master slave
#define SPI_ROLE_MASTER 1
#define SPI_ROLE_SLAVE 0
// Clock polarity
#define SPI_CLOCK_POLARITY_IDLE_HIGH 1
#define SPI_CLOCK_POLARITY_IDLE_LOW 0
// Clock phase
#define SPI_CLOCK_PHASE_SAMPLE_ON_TRAILING 1
#define SPI_CLOCK_PHASE_SAMPLE_ON_LEADING 0
// Clock rate
#define SPI_CLOCK_RATE_FOSC_2 4
#define SPI_CLOCK_RATE_FOSC_4 0
#define SPI_CLOCK_RATE_FOSC_8 5
#define SPI_CLOCK_RATE_FOSC_16 1
#define SPI_CLOCK_RATE_FOSC_32 6
#define SPI_CLOCK_RATE_FOSC_64 2
#define SPI_CLOCK_RATE_FOSC_128 3

/**
 * @brief Initialize SPI channel
 * @param config Configuration for spi channel
 */
extern void spi_init(spi_config_t *config);

/**
 * @brief Get default spi config
 * @param config Configuration to set to default values
 * @see spi_config_t
 */
extern void spi_get_default_config(spi_config_t *config);

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

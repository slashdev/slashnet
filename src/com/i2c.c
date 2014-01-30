/**
 * @file i2c.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.11.0
 */

#include "i2c.h"

// Do we want I2C?
#ifdef COM_I2C

void i2c_init(uint16_t bitrateKHz) {
    
    // Disable power reduction mode
    PRR &= ~(1<<PRTWI);
    
    uint8_t prescaler;
    
    // Clear prescaler bits
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
    
    // Decide which prescaler to use
    if (bitrateKHz < 3) {
        prescaler = 64;
        TWSR = (1 << TWPS1) | (1 << TWPS0);
    } else if (bitrateKHz < 10) {
        prescaler = 16;
        TWSR |= (1 << TWPS1);
    } else if (bitrateKHz < 38) {
        prescaler = 4;
        TWSR |= (1 << TWPS0);
    } else {
        prescaler = 1;
    }
    
    // Calculate bitrate divider
    // SCL = F_CPU / (16 + 2(div) * prescaler)
    // div = (F_CPU / (2001*SCL*prescaler))
    TWBR = (F_CPU / 2001 / bitrateKHz / prescaler) - 7;
    
    // Enable TWI
    TWCR |= (1 << TWEN);
}

void i2c_disable(void) {
    TWCR &= ~(1 << TWEN);
}

#if defined(UTILS_LOGGER_INFO) || defined(UTILS_LOGGER_DEBUG)
void error(const char *msg) {
    info_string_p(PSTR("I2C error: "));
    info_string_p(msg);
    info_newline();
#ifdef UTILS_LOGGER_DEBUG
    debug_string_p(PSTR("TWSR: 0x"));
    debug_number_as_hex(TWSR);
    debug_newline();
#endif // UTILS_LOGGER_DEBUG
}
#else // UTILS_LOGGER_INFO || UTILS_LOGGER_DEBUG
#define error(...) do {} while (0)
#endif // UTILS_LOGGER_INFO || UTILS_LOGGER_DEBUG

const uint16_t default_timeout = 0x4000;

// Used for every operation except stop
// Returns false on timeout
uint8_t timeout(uint16_t timeout) {
    // Wait for I2C to set TWINT
    while ((!(TWCR & (1 << TWINT))) && timeout-- > 0) { }
    // Return status
    return timeout <= 0;
}

void restart(void) {
    // Release SCL and SDA
    TWCR = 0;
    // Wait 2 usec
    _delay_us(2);
    // Restart
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(void) {
    // Make sure at least 1.3 usec is between stop and start
    _delay_us(2);
    // Send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // Wait until start has been acked
    if (timeout(default_timeout)) {
        // Timeout, restart
        restart();
        return (1);
    }
    // No timeout, check status
    if ((TW_STATUS == TW_START) || (TW_STATUS == TW_REP_START)) {
        return (0);
    } else if (TW_STATUS == TW_MT_ARB_LOST) {
        uint8_t status = TW_STATUS;
        restart();
        return (status);
    }
    return (TW_STATUS);
}

uint8_t i2c_stop(void) {
    // Send stop condition
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    // Wait for stop to complete
    uint16_t timeout = default_timeout;
    while (TWCR & (1 << TWSTO)) {
        if (timeout-- <= 0) {
            // Exit error
            restart();
            return (1);
        }
    }
    // Exit ok
    return (0);
}

#endif // COM_I2C

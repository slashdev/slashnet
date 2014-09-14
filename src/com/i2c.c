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

uint8_t current_bitrateKHz;

void i2c_init(uint16_t bitrateKHz) {

    // Disable power reduction mode
    PRR &= ~(1<<PRTWI);

    if (bitrateKHz != current_bitrateKHz) {
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

        // Set current bitrate
        current_bitrateKHz = bitrateKHz;
    }

    // Enable TWI
    TWCR |= (1 << TWEN);
}

void i2c_disable(void) {
    TWCR &= ~(1 << TWEN);
}

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

uint8_t i2c_send_address(uint8_t address) {
    // Set address in register
    TWDR = address;
    // Start sending
    TWCR = (1 << TWINT) | (1 << TWEN);
    // Wait until send completes
    if (timeout(default_timeout)) {
        // Timeout
        restart();
        return (1);
    }
    // Check status
    uint8_t status = TW_STATUS;
    if ((status == TW_MT_SLA_ACK) || (status == TW_MR_SLA_ACK)) {
        // Received ack
        return (0);
    } else if ((status == TW_MT_SLA_NACK) || (status == TW_MR_SLA_NACK)) {
        // Received nack
        i2c_stop();
        return (status);
    } else {
        // Faulty
        restart();
        return (status);
    }
}

uint8_t i2c_send_byte(uint8_t byte) {
    // Put byte into register
    TWDR = byte;
    // Send byte
    TWCR = (1 << TWINT) | (1 << TWEN);
    // Wait until send completes
    if (timeout(default_timeout)) {
        // Timeout
        restart();
        return (1);
    }
    // Check status
    uint8_t status = TW_STATUS;
    if (status == TW_MT_DATA_ACK) {
        // Received ack
        return (0);
    } else if (status == TW_MT_DATA_NACK) {
        // Received nack
        i2c_stop();
        return (status);
    } else {
        // Faulty
        restart();
        return (status);
    }
}

uint8_t i2c_write_register(uint8_t address, uint8_t reg, uint8_t byte) {
    // Start i2c
    if (i2c_start()) { return 1; }
    // Select address
    else if (i2c_send_address(I2C_WRITE(address))) { return 1; }
    // Select register
    else if (i2c_send_byte(reg)) { return 1; }
    // Send new byte
    else if (i2c_send_byte(byte)) { return 1; }
    // Stop i2c
    i2c_stop();
    return 0;
}

uint8_t i2c_receive_byte(uint8_t ack, uint8_t *data) {
    // Prepare for byte
    if (ack) {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    } else {
        TWCR = (1 << TWINT) | (1 << TWEN);
    }
    // Wait until byte is received
    if (timeout(default_timeout)) {
        // Timeout
        restart();
        return (1);
    }
    // Check status
    uint8_t status = TW_STATUS;
    if (status == TW_MR_DATA_ACK || status == TW_MR_DATA_NACK) {
        *data = TWDR;
        return (0);
    } else if (status == TW_MT_ARB_LOST) {
        restart();
        return (status);
    } else {
        return (status);
    }
}

#endif // COM_I2C

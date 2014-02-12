/**
 * @file tlc59116.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.12.0
 */

#include "tlc59116.h"

// Do we want TLC59116?
#ifdef EXT_TLC59116

// Check if COM_I2C is enabled
#ifndef COM_I2C
#error TLC59116 cannot work without COM_I2C
#endif // COM_I2C

/**
 * Speed for I2C to talk over
 */
const uint16_t i2c_speed = 350;

void tlc59116_reset(void) {
    // Make sure i2c is initialized
    i2c_init(i2c_speed);
    // Start i2c
    if (i2c_start()) { /* Start failed */ }
    // Send reset command
    else if(i2c_send_address(0xD6)) { /* Send reset command failed */ }
    // Send first reset byte
    else if(i2c_send_byte(0xA5)) { /* Send first reset byte failed */ }
    // Send second reset byte
    else if(i2c_send_byte(0x5A)) { /* Send second reset byte failed */ }
    // Stop i2c
    i2c_stop();
}

void tlc59116_sleep(uint8_t address, uint8_t sleep) {
    // Sequence:
    // Start, select chip, select register, repeated start, select chip,
    // read register (not ack), repeated start, select chip, select register,
    // write new byte (sleep or not), stop

    // Make sure i2c is initialized
    i2c_init(i2c_speed);
    // Start i2c
    i2c_start();
    // Select chip (WRITE)
    i2c_send_address(I2C_WRITE(address));
    // Select register
    i2c_send_byte(0x00);
    // Repeated start
    i2c_start();
    // Select chip (READ)
    i2c_send_address(I2C_READ(address));
    // Read byte (do not ack)
    uint8_t old_state;
    i2c_receive_byte(0, &old_state);
    // Repeated start
    i2c_start();
    // Select chip (WRITE)
    i2c_send_address(I2C_WRITE(address));
    // Select register
    i2c_send_byte(0x00);
    // Write byte
    if (sleep) {
        i2c_send_byte(0x10);
    } else {
        i2c_send_byte(old_state & ~(0x10));
    }
    // Stop
    i2c_stop();
    // Wait for 500us to let chip start crystal
    _delay_us(500);
}

void tlc59116_set_led_mode(uint8_t address, uint8_t led_nr, uint8_t mode) {
    // Sequence
    // Start, select chip, select register, repeated start, select chip
    // read register (not ack), repeated start, select chip, select register,
    // write new byte (on or off), stop

    // Determine register and mask
    uint8_t reg  = 0x14 + ((led_nr % 16) / 4);
    uint8_t shift = (led_nr % 4) * 2;

    // Make sure I2C is initialized
    i2c_init(i2c_speed);
    // Start i2c
    i2c_start();
    // Select chip (WRITE)
    i2c_send_address(I2C_WRITE(address));
    // Select register
    i2c_send_byte(reg);
    // Repeated start
    i2c_start();
    // Select chip (READ)
    i2c_send_address(I2C_READ(address));
    // Read byte (do not ack)
    uint8_t state;
    i2c_receive_byte(0, &state);
    // Repeated start
    i2c_start();
    // Select chip (WRITE)
    i2c_send_address(I2C_WRITE(address));
    // Select register
    i2c_send_byte(reg);
    // Write new value
    state &= ~(0x03 << shift);
    state |= mode << shift;
    i2c_send_byte(state);
    // Stop
    i2c_stop();
}

void tlc59116_write_register(uint8_t address, uint8_t reg, uint8_t byte) {
    if (reg > 0x1B) {
        return; // Register out of bounds
    }
    // Make sure i2c is initialized
    i2c_init(i2c_speed);
    // Write to register
    i2c_write_register(address, reg, byte);
}

void tlc59116_set_brightness(uint8_t address, uint8_t led_nr, uint8_t brightness) {
    if (led_nr >= 16) {
        return; // Led should be in range 0 - 15
    }
    // Make sure i2c is initialized
    i2c_init(i2c_speed);
    // Write to register
    tlc59116_write_register(address, 0x02 + led_nr, brightness);
}

void tlc59116_set_brightness_array(uint8_t address, uint8_t start_led_nr, uint8_t length, uint8_t *data) {
    // Sequence
    // Start, select chip, select start register, write new bytes, stop

    // Make sure i2c is initialized
    i2c_init(i2c_speed);
    // Start i2c
    i2c_start();
    // Select chip (WRITE)
    i2c_send_address(I2C_WRITE(address));
    // Select start register (auto increase over brightness registers only)
    i2c_send_byte(0xA2 + (start_led_nr % 16));
    // Iterate over data
    while (length--) {
        // Send new byte
        i2c_send_byte((*data) % 0xFF);
        data++;
    }
    // Stop
    i2c_stop();

}

#endif // EXT_TLC59116

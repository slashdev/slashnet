/**
 * @file network.h
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.3.0
 */

#include "network.h"

// Do we want network?
#ifdef NET_NETWORK

// Check if F_CPU is set
#ifndef F_CPU
#error F_CPU is not set but required for network
#endif // F_CPU

// Defines
// ---------

// Port and data direction register
#define NETWORK_PORT PORTB
#define NETWORK_DDR  DDRB

// Control pins
#define NETWORK_CTR_CS  PORTB4
#define NETWORK_CTR_SI  PORTB5
#define NETWORK_CTR_SO  PORTB6
#define NETWORK_CTR_SCK PORTB7

// Enable or disable SPI selector
#define NETWORK_SPI_ACTIVE() SPI_ACTIVE(NETWORK_PORT, NETWORK_CTR_CS)
#define NETWORK_SPI_PASSIVE() SPI_PASSIVE(NETWORK_PORT, NETWORK_CTR_CS)

// Variables
// ---------

// External addresses from config
extern uint8_t my_ip[];
extern uint8_t my_mac[];

// Current register
static uint8_t bank;
// Next packet pointer
static uint16_t next_packet_ptr;

// Buffer for recieved packets
uint8_t buffer_in[BUFFER_IN_SIZE+1];
// Buffer for packets to transmit
uint8_t buffer_out[BUFFER_OUT_SIZE];
// Length of received packet
uint16_t buffer_in_length;

// Functions
// ---------

// Read and write registers
uint8_t read_op(uint8_t op, uint8_t address);
void    write_op(uint8_t op, uint8_t address, uint8_t data);

// Read and write buffers
void    read_buffer(uint16_t length, uint8_t *buffer);
void    write_buffer(uint16_t length, uint8_t *buffer);

// Read and write registers with bank check
uint8_t read(uint8_t address);
void    write(uint8_t address, uint8_t data);

// Read and write PHY registes
uint8_t read_phy_high(uint8_t address);
uint8_t read_phy_low(uint8_t address);
void    write_phy(uint8_t address, uint16_t data);

// Helpers
void    set_bank(uint8_t address);
uint8_t get_revision(void);

//
// Read / write registers
//

uint8_t read_op(uint8_t op, uint8_t address) {
    // Claim spi
    NETWORK_SPI_ACTIVE();
    // Issue read command
    SPDR = op | (address & ADDR_MASK);
    // Wait for spi to finish
    SPI_WAIT();
    // Read data
    SPDR = 0x00;
    SPI_WAIT();
    // Do dummy read when needed (for mac address and mii)
    if (address & 0x80) {
        SPDR = 0x00;
        SPI_WAIT();
    }
    // Release spi
    NETWORK_SPI_PASSIVE();
    return (SPDR);
}

void write_op(uint8_t op, uint8_t address, uint8_t data) {
    // Claim spi
    NETWORK_SPI_ACTIVE();
    // Issue write command
    SPDR = op | (address & ADDR_MASK);
    SPI_WAIT();
    // Write data
    SPDR = data;
    SPI_WAIT();
    // Release spi
    NETWORK_SPI_PASSIVE();
}

//
// Read and write buffers
//

void read_buffer(uint16_t length, uint8_t *buffer) {
    // Claim spi
    NETWORK_SPI_ACTIVE();
    // Issue read command
    SPDR = NETWORK_READ_BUF_MEM;
    SPI_WAIT();
    // While length available, read buffer
    while (length) {
        length--;
        // Read buffer
        SPDR = 0x00;
        SPI_WAIT();
        *buffer = (SPDR);
        buffer++;
    }
    // Ensure null terminator
    *buffer = '\0';
    // Release spi
    NETWORK_SPI_PASSIVE();
}

void write_buffer(uint16_t length, uint8_t *buffer) {
    // Claim spi
    NETWORK_SPI_ACTIVE();
    // Issue write command
    SPDR = NETWORK_WRITE_BUF_MEM;
    SPI_WAIT();
    // While length available, write buffer
    while (length) {
        length--;
        // Write data
        SPDR = *buffer;
        buffer++;
        SPI_WAIT();
    }
    // Release spi
    NETWORK_SPI_PASSIVE();
}

//
// Read and write registers with bank check
//

uint8_t read(uint8_t address) {
    // Set bank
    set_bank(address);
    // Read data
    return read_op(NETWORK_READ_CTRL_REG, address);
}

void write(uint8_t address, uint8_t data) {
    // Set bank
    set_bank(address);
    // Write data
    write_op(NETWORK_WRITE_CTRL_REG, address, data);
}

//
// Read and write PHY registes
//

uint8_t read_phy_high(uint8_t address) {
    // Set the right address and start the register read operation
    write(MIREGADR, address);
    write(MICMD, MICMD_MIIRD);
    // Wait 10us
    _delay_loop_2(0);
    // Wait until PHY read completes
    while (read(MISTAT) & MISTAT_BUSY)
        ;
    // Reset reading bit
    write(MICMD, 0x00);
    return (read(MIRDH));
}

uint8_t read_phy_low(uint8_t address) {
    // Set the right address and start the register read operation
    write(MIREGADR, address);
    write(MICMD, MICMD_MIIRD);
    // Wait 10us
    _delay_loop_2(0);
    // Wait until PHY read completes
    while (read(MISTAT) & MISTAT_BUSY)
        ;
    // Reset reading bit
    write(MICMD, 0x00);
    return (read(MIRDL));
}

void write_phy(uint8_t address, uint16_t data) {
    // Set the PHY register address
    write(MIREGADR, address);
    // Write PHY data
    write(MIWRL, data & 0xFF);
    write(MIWRH, data >> 8);
    // Wait until the PHY write completes
    while (read(MISTAT) & MISTAT_BUSY) {
        // Wait 10us
        _delay_loop_2(0);
    }
}

uint8_t network_status(void) {
    uint8_t status = read_phy_high(PHSTAT2) << 2;
    status |= (read_phy_low(PHSTAT2) >> 5) & 0x01;
    status |= read_phy_low(PHSTAT1);
    return status;
}

//
// Helpers
//

void set_bank(uint8_t address) {
    // Change bank address when needed
    if ((address & BANK_MASK) != bank) {
        // Change bank
        bank = (address & BANK_MASK);
        write_op(NETWORK_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
        write_op(NETWORK_BIT_FIELD_SET, ECON1, (bank >> 5));
    }
}

uint8_t get_revision(void) {
    uint8_t revision;
    revision = read(EREVID);
    // Microchip forgot to step the number on the silicon when they
    // released the revision B7. 6 is now rev B7. We still have to
    // see what they do when they release B8. At the moment there
    // is no B8 out yet.
    if (revision > 5)
        revision++;
    return (revision);
}

uint8_t network_is_link_up(void) {
    // bit 10 (3rd bit in upper register)
    if (read_phy_high(PHSTAT2) & 0x04) {
        return (1);
    }
    return (0);
}

//
// Network functions
//

void network_init(void) {
    // Notify debug
    debug_string_p(PSTR("\r\nInit network\r\n"));
    
    // Initialize spi
    // --------------
    // Initialize IO for SPI communication
    NETWORK_DDR |= 0
    | (1 << NETWORK_CTR_CS)   // SS as output
    | (1 << NETWORK_CTR_SI)   // SI as output
    | (1 << NETWORK_CTR_SCK); // SCK as output
    NETWORK_DDR &= ~(1 << NETWORK_CTR_SO); // Explicitly make SO input
    
    // Set SPI to passive
    NETWORK_SPI_PASSIVE();
    
    // Set SI and SCK to LOW
    NETWORK_PORT |= 0
    | (1 << NETWORK_CTR_SI)
    | (1 << NETWORK_CTR_SCK);
    
    // Spi config
    // - Enable
    // - Master role
    // - Fosc / 2 clock rate
    spi_config_t spi_config;
    spi_config.enable = 1;
    spi_config.role = SPI_ROLE_MASTER;
    spi_config.clock_rate = SPI_CLOCK_RATE_FOSC_2;
    spi_init(&spi_config);
    
    
    // Soft reset
    // ----------
    
    // Do a soft reset of the chip
    //write_op(NETWORK_SOFT_RESET, 0, NETWORK_SOFT_RESET);
    // Wait for 20ms
    //_delay_ms(20);
}

#endif // NET_NETWORK

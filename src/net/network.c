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
void     set_bank(uint8_t address);
uint8_t  get_revision(void);
uint16_t network_receive(void);

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
    debug_string_p(PSTR("Init network\r\n"));
    
    // Initialize spi
    // --------------
    // Initialize IO for SPI communication
    NETWORK_DDR |=
      (1 << NETWORK_CTR_CS)   // SS as output
    | (1 << NETWORK_CTR_SI)   // SI as output
    | (1 << NETWORK_CTR_SCK); // SCK as output
    NETWORK_DDR &= ~(1 << NETWORK_CTR_SO); // Explicitly make SO input
    
    // Set SPI to passive
    NETWORK_SPI_PASSIVE();
    
    // Set SI and SCK to LOW
    NETWORK_PORT |=
      (1 << NETWORK_CTR_SI)
    | (1 << NETWORK_CTR_SCK);
    
    // Spi config
    // - Enable
    // - Master role
    spi_config_t spi_config;
    spi_get_default_config(&spi_config);
    spi_config.enable = SPI_ENABLE;
    spi_config.role = SPI_ROLE_MASTER;
    spi_init(&spi_config);
    
    
    // Soft reset
    // ----------
    
    // Do a soft reset of the chip
    write_op(NETWORK_SOFT_RESET, 0, NETWORK_SOFT_RESET);
    // Wait for 20ms
    _delay_ms(20);
    
    // Initiate bank 0 settings
    // ------------------------
    // Transmit and receive buffer settings
    
    // Tx start
    write(ETXSTL, TXSTART_INIT & 0xFF);
    write(ETXSTH, TXSTART_INIT >> 8);
    
    // Tx stop
    write(ETXNDL, TXSTOP_INIT & 0xFF);
    write(ETXNDH, TXSTOP_INIT >> 8);
    
    // Set receive buffer start address
    next_packet_ptr = RXSTART_INIT;
    
    // Rx start
    write(ERXSTL, RXSTART_INIT & 0xFF);
    write(ERXSTH, RXSTART_INIT >> 8);
    
    // Rx stop
    write(ERXNDL, RXSTOP_INIT & 0xFF);
    write(ERXNDH, RXSTOP_INIT >> 8);
    
    // Set receive pointer
    write(ERXRDPTL, RXSTART_INIT & 0xFF);
    write(ERXRDPTH, RXSTART_INIT >> 8);
    
    // Initiate bank 1 settings
    // ------------------------
    // Packet filters
    
    // Set filters
    write(ERXFCON,
          ERXFCON_UCEN   // Unicast enabled, only accept matching local mac
        | ERXFCON_CRCEN  // Check CRC post filter
        | ERXFCON_PMEN); // Enable packet pattern match filter
    
    // The packet pattern match filter allows arp broadcast packets
    // The pattern to match is:
    // Type   Eth Destination
    // ARP    Broadcast
    // 06 08  FF FF FF FF FF FF
    // IP checksum for these bytes is F7F9
    // In binary these positions are: 11 0000 0011 1111
    // This is hex 303F
    write(EPMM0, 0x3F);
    write(EPMM1, 0x30);
    write(EPMCSL, 0xF9);
    write(EPMCSH, 0xF7);
    
    
    // Initiate bank 2 settings
    // ------------------------
    
    // Enable mac receive
    write(MACON1,
          MACON1_MARXEN   // Mac receive enable
        | MACON1_TXPAUS   // Pause control transmit enable
        | MACON1_RXPAUS); // Pause control receive enable
    
    // Bring mac out of reset
    write(MACON2, 0x00);
    
    // Enable automatic padding to 60 bytes and CRC operations
    write(MACON3,
          MACON3_PADCFG0   // Short frames will be padded to 60 bytes and crc appended
        | MACON3_TXCRCEN   // Transmit CRC enable
        | MACON3_FRMLNEN); // Frame length check enable
    
    // No options of macon4 should be used
    write(MACON4, 0x00);
    
    // Set inter-frame gap (back-to-back)
    write(MABBIPG, 0x12); // Half-duplex value
    
    // Set inter-frame gap (non back-to-back)
    write(MAIPGL, 0x12);
    write(MAIPGH, 0x0C);
    
    // Set the maximum packet size which the chip will accept
    // Do not recieve packets larger than BUFFER_IN_SIZE
    write(MAMXFLL, BUFFER_IN_SIZE & 0xFF);
    write(MAMXFLH, BUFFER_IN_SIZE >> 8);
    
    
    // Initialize bank 3 settings
    // --------------------------
    // Mac adress
    
    // Set mac address
    write(MAADR0, my_mac[0]);
    write(MAADR1, my_mac[1]);
    write(MAADR2, my_mac[2]);
    write(MAADR3, my_mac[3]);
    write(MAADR4, my_mac[4]);
    write(MAADR5, my_mac[5]);
    
    
    // PHY bank settings
    // -----------------
    
    // Disable loopback of transmitted frames
    write_phy(PHCON2, PHCON2_HDLDIS);
    
    // Magjack led configuration
    // See datasheet p. 11
    // Led a = link status, led b = receive/transmit => 0x476
    write_phy(PHLCON, 0x476);
    
    // General settings
    // ----------------
    
    // Set bank to 0
    set_bank(ECON1);
    
    // Enable interrupts
    write_op(NETWORK_BIT_FIELD_SET, EIE,
          EIE_INTIE   // Global interrupt enable
        | EIE_PKTIE); // Receive packet pending interrupt enable
    
    // Enable packet reception
    write_op(NETWORK_BIT_FIELD_SET, ECON1, ECON1_RXEN); // Receive enable
    
    // Disable clock output
    write(ECOCON, 0x00);
    
    // Wait ~60 us
    _delay_us(60);
    
    debug_string_p(PSTR("Init network finished\r\n"));
    
    // Init arp
    // Init udp
    // Init counter
    // Init DHCP
}

void network_backbone(void) {
    // Check if there is a packet available
    network_receive();
    // Check if a DHCP packet is received
    //dhcp_renew_handler
    // If there is no buffer_in_length, there is no packet
    if (buffer_in_length == 0) {
        return;
    }
    // Handle protocols
    // arp, icmp, udp, tcp
}

void network_send(uint16_t length) {
    // Check if there is a transmission in progress
    while (read_op(NETWORK_READ_CTRL_REG, ECON1) & ECON1_TXRTS) {
        // Reset the transmission logic problem.
        // See revision B4 silicon errata point 12
        if (read_op(NETWORK_READ_CTRL_REG, EIR) & EIR_TXERIF) {
            write_op(NETWORK_BIT_FIELD_SET, ECON1, ECON1_TXRST);
            write_op(NETWORK_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        }
    }
    
    // Set the write pointer to the start of the transmit buffer area
    write(EWRPTL, TXSTART_INIT & 0xFF);
    write(EWRPTH, TXSTART_INIT >> 8);
    
    // Set the TXND pointer to correspond the packet size given
    write(ETXNDL, (TXSTART_INIT + length) & 0xFF);
    write(ETXNDH, (TXSTART_INIT + length) >> 8);
    
    // Write per-packet control byte (0x00 means use MACON3 settings)
    write_op(NETWORK_WRITE_BUF_MEM, 0, 0x00);
    
    // Write data to transmit buffer
    write_buffer(length, buffer_out);
    
    // Send the packet onto the network
    write_op(NETWORK_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

uint16_t network_receive(void) {
    return 0;
}

#endif // NET_NETWORK

/**
 * Copyright (C) Ferdi van der Werf. All rights reserved
 *
 * This file is part of EthShield, distributed under the MIT license.
 */

#include "uart.h"

// Do we want UART?
#ifdef COM_UART

// Check if F_CPU is set
#ifndef F_CPU
#error F_CPU is not set but required for UART
#endif // F_CPU

// Define buffer ring size if not defined
#ifndef COM_UART_BUFFER_RING_SIZE
#define COM_UART_BUFFER_RING_SIZE 64
#endif // COM_UART_BUFFER_RING_SIZE

// Buffer ring struct
typedef struct ringbuffer {
    uint8_t buffer[COM_UART_BUFFER_RING_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} ringbuffer_t;

// Create buffer ring
ringbuffer_t com_uart_tx_buffer_ring = { { 0 }, 0, 0 };

void uart_init(uart_config_t *config) {
    // Disable transmit and receive
    UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
    
    // Set baudrate
    uint16_t brr = ((F_CPU / (8UL * config->baudrate)) -1);
    UBRR0H = brr >> 8;
    UBRR0L = brr & 0xFF;
    
    // Set UCSR0A register
    // Set double speed
    if (config->doublespeed) {
        UCSR0A |= (1 << U2X0);
    } else {
        UCSR0A &= ~(1 << U2X0);
    }
    
    // Set multi-processor mode
    if (config->multiprocessormode) {
        UCSR0A |= (1 << MPCM0);
    } else {
        UCSR0A &= ~(1 << MPCM0);
    }
    
    // Set UCSR0B register
    // Receive enable/disable
    if (config->enable_rx) {
        OCSR0B |= (1 << RXEN0);
    } else {
        UCSR0B &= ~(1 << RXEN0);
    }
    
    // Transmit enable/disable
    // Set interrupt accordingly
    if (config->enable_tx) {
        UCSR0B |= (1 << TXEN0) | (1 << UDRIE0);
    } else {
        UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0));
    }
    
}

void uart_set_default_config(uart_config_t *config) {
    // Make sure we have a valid pointer
    if (!config)
        return;
    
    config->baudrate = 9600;
    config->doublespeed = 0;
    config->multiprocessormode = 0;
    config->enable_rx = 1;
    config->enable_tx = 1;
}

// Send if tx buffer has contents
ISR(USART0_UDRE_vect) {
    if (com_uart_tx_buffer_ring.head == com_uart_tx_buffer_ring.tail) {
        // Buffer empty, disable interrupt
        OCSR0B &= ~(1 << UDRIE0);
    } else {
        // Get data to send
        uint8_t value = com_uart_tx_buffer_ring.buffer[com_uart_tx_buffer_ring.tail];
        com_uart_tx_buffer_ring = (com_uart_tx_buffer_ring.tail + 1) % COM_UART_BUFFER_RING_SIZE;
        // Send next byte
        UDR0 = value;
    }
}

void uart_send(uint8_t data) {
    // Get index + 1 of buffer head
    uint8_t index = (com_uart_tx_buffer_ring.head + 1) % COM_UART_BUFFER_RING_SIZE;
    
    // If the tx buffer is full, wait until interrupt handler empties
    // a slot in the buffer to write to.
    while (index == com_uart_tx_buffer_ring.tail) {
        continue;
    }
    
    // Write the byte
    com_uart_tx_buffer_ring.buffer[com_uart_tx_buffer_ring.head] = data;
    com_uart_tx_buffer_ring.head = index;
    
    // Set data registry empty bit
    OCSR0B |= (1 << UDRIE0);
    // Clear the transmit complete bit
    UCSR0A &= ~(1 << TXC0);
}

void uart_send_string(uint8_t *data) {
    while (*data) {
        uart_write(*data++);
    }
}

void uart_send_string_p(const uint8_t *pdata) {
    char c;
    while ((c = pgm_read_byte(pdata++))) {
        uart_write(c);
    }
}

#endif // COM_UART

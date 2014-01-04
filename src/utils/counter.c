/**
 * @file counter.c
 *
 * \copyright Copyright 2013 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.1.0
 */

#include "counter.h"

// Do we want to use the counter?
#ifdef UTILS_COUNTER

// Check if a timer is selected
#if !defined(UTILS_COUNTER_TIMER0) && !defined(UTILS_COUNTER_TIMER1) && !defined(UTILS_COUNTER_TIMER2)
#error No timer selected for counter, it should be configured with a single timer
#endif
// Check if multiple timers have been selected
#if (defined(UTILS_COUNTER_TIMER0) && (defined(UTILS_COUNTER_TIMER1) || defined(UTILS_COUNTER_TIMER2))) || (defined(UTILS_COUNTER_TIMER1) && defined(UTILS_COUNTER_TIMER2))
#error Multiple timers selected for counter, it should be configured with only a single timer
#endif

// Counter which shows how far in a second we are
volatile uint8_t sub_seconds = 0;
#ifdef ETH_DHCP
// Current second in dhcp mechanics
extern volatile uint8_t dhcpSeconds;
#endif


void cntTick(void) {
#ifdef ETH_DHCP
    // Update dhcp counter
    dhcpSeconds++;
    PORTC = dhcpSeconds;
#endif
#ifdef API_UPTIME
    // Update uptime counter
    uptimeTick();
#endif
}

// For each timer selection cntInit() and overflow interrupt will be created
#if defined(UTILS_COUNTER_TIMER0)
void cntInit(void) {
    // 8 bit timer
    // 20 Mhz, prescaler 1024, compare match on 0xD9
    // Count to 11 for a second
    // In 2 minutes, it is ~4 seconds too fast
    
    // CTC operation, OC0A and OC0B disconnected
    TCCR0A = (1 << WGM01);
    // Prescaler 1024
    TCCR0B = (1 << CS00) | (1 << CS02);
    // Compare match value
    OCR0A = 0xD9;
    // Set compare interrupt
    TIMSK0 = (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
    sub_seconds++;
    if (sub_seconds >= 89) {
        // A second passed
        cntTick();
        sub_seconds = 0;
    }
}

#elif defined(UTILS_COUNTER_TIMER1)
void cntInit(void) {
    // 16 bit timer
    // 20 Mhz, prescaler 1024, compare match on 0x4C4A
    
    // CTC operation, OC1A and OC1B disconnected, prescaler 1024
    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
    // Compare match value
    OCR1A = 0x4C48;
    // Set compare interrupt
    TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    cntTick();
}

#elif defined(UTILS_COUNTER_TIMER2)
void cntInit(void) {
    // 8 bit timer
    // 20 Mhz, prescaler 1024, compare match on 0xD9
    // Count to 11 for a second
    // In 2 minutes, it is ~4 seconds too fast
    
    // CTC operation, OC2A and OC2B disconnected
    TCCR2A = (1 << WGM21);
    // Prescaler 1024
    TCCR2B = (1 << CS20) | (1 << CS21) | (1 << CS22);
    // Compare match value
    OCR2A = 0xD9;
    // Set compare interrupt
    TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
    sub_seconds++;
    if (sub_seconds >= 89) {
        // A second passed
        cntTick();
        sub_seconds = 0;
    }
}
#endif

#endif // UTIL_COUNTER

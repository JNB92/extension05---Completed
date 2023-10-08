#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "qutyserial.h"
#include <avr/interrupt.h>

#define FREQ1 2140 // Frequency for f1
#define FREQ2 4000 // Frequency for f2
#define TIME1 300  // Duration for f1
#define TIME2 670  // Duration for f2

volatile uint8_t sirenState = 0; // 0 for f1, 1 for f2

/** EX: E5.0


TASK:

Your task is to write code to implement a two-tone siren
using the QUTy. You must use one of the Waveform Outputs
of TCA0 to drive the buzzer. The buzzer should always
be driven with a 50% duty cycle signal.

Your siren must alternate between the following frequencies:

    f1 = 2AB0 Hz with f1 active for t1 = 3E0 ms
    f2 = 4CD0 Hz with f2 active for t2 = 6F0 ms

where A-F are the 2nd through 7th digits of your student
number:

    nXABCDEFX

EXAMPLE: If your student number was n12345678, then

A = 2, B = 3, C = 4, D = 5, E = 6, F = 7

f1 = 2230 Hz
f2 = 4450 Hz
t1 = 360 ms
t2 = 670 ms

NOTE: Your programme must be interrupt driven.

main() will call the init() function you write below
and then drop into an infinite loop. The init() function
you write MUST return to demonstrate your code is
interrupt driven.

In addition to init(), you may write any code you wish
in this file, including ISRs.
*/

void init(void)
{
    
    
    // Configure buzzer net as an output
    PORTB.DIRSET = PIN0_bm;  // Assuming buzzer is connected to PIN1 of PORTB

    // Set up TCA0 in single slope mode
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

    // Set up TCA0 to use the system clock (no prescaling)
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm; // ENABLE COMPARE CHANNEL 0
    
    // Enable overflow interrupt for TCA0
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

    // Set initial frequency to FREQ1
    TCA0.SINGLE.PERBUF = (uint16_t)(F_CPU / (2 * FREQ1) - 1);

    // Start TCA0 and set prescaler to F_CPU/1 (no prescaling)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV1_gc;

    
}

// TCA0 overflow ISR
ISR(TCA0_OVF_vect) 

{
    uint16_t overflow_count = 0;  // Number of overflows since the last state change
    uint16_t overflows_required;
    if (sirenState == 0) {
        // Set frequency to FREQ2 and duration to TIME2
        TCA0.SINGLE.PERBUF = (uint16_t)(F_CPU / (2 * FREQ2) - 1);
        TCA0.SINGLE.CMP0BUF = (TCA0.SINGLE.PERBUF / 2); // 50% duty cycle
        TCA0.SINGLE.CNT = 0;
        
        // Calculate number of overflows required to reach TIME2 duration
        overflows_required = 10;  // assuming TIME2 is in milliseconds

        // Increment the overflow counter
        overflow_count++;

        // Check if it's time to switch to the next state
        if (overflow_count >= overflows_required) {
            sirenState = 1;
            overflow_count = 0;  // Reset overflow counter for the next state
    } else {
    // Set frequency to FREQ1 and duration to TIME1
    TCA0.SINGLE.PERBUF = (uint16_t)(F_CPU / (2 * FREQ1) - 1);
    TCA0.SINGLE.CMP0BUF = (TCA0.SINGLE.PERBUF / 2); // 50% duty cycle
    TCA0.SINGLE.CNT = 0;

    // Calculate number of overflows required to reach TIME1 duration
    overflows_required = 10;  // assuming TIME1 is in milliseconds

    // Increment the overflow counter
    overflow_count++;

    // Check if it's time to switch to the next state
    if (overflow_count >= overflows_required) {
        sirenState = 0;
        overflow_count = 0;  // Reset overflow counter for the next state
    }

}
// Clear the interrupt flag
TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    }
}
/** CODE: Write your code for Ex E5.0 above this line. */
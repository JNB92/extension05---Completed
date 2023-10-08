#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "qutyserial.h"
#include <avr/interrupt.h>

#define FREQ1 1556 // Frequency for f1
#define FREQ2 833 // Frequency for f2
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
    cli(); // Disable interrupts
    PORTB.DIRSET = PIN0_bm; // Set PB0 as output

    
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // Set single slope PWM mode

   
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm; // Enable compare channel 0

    
    TCA0.SINGLE.PERBUF = FREQ1; // Set period for f1
    TCA0.SINGLE.CMP0BUF = FREQ1 / 2; // Set compare value for 50% duty cycle
    TCA0.SINGLE.CNT = 0; // Set counter to 0

    
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Enable overflow interrupt

    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV1_gc; // Enable TCA0 with no prescaler
    sei(); // Enable interrupts

}

void TCA0_setup(void){
    
}

ISR(TCA0_OVF_vect) 
{
    static uint16_t overflow_count = 0; // Number of overflows since last toggle
    uint16_t overflows_required; // Number of overflows required for next toggle

    
    if (sirenState == 0) // If f1 is active
    {
        
        TCA0.SINGLE.PERBUF = FREQ2; // Set period for f2
        TCA0.SINGLE.CMP0BUF = FREQ2 / 2; // Set compare value for 50% duty cycle
        TCA0.SINGLE.CNT = 0; // Reset counter
        
        
        overflows_required = 641; // 2140 Hz * 0.3 s = 642 overflows
    } 
    
    else 
    {
        
        TCA0.SINGLE.PERBUF = FREQ1; // Set period for f1
        TCA0.SINGLE.CMP0BUF = FREQ2 / 2; // Set compare value for 50% duty cycle
        TCA0.SINGLE.CNT = 0; // Reset counter

       
        overflows_required = 2675; // 4000 Hz * 0.67 s = 2680 overflows
    }

  
    overflow_count++; // Increment overflow count

    
    if (overflow_count >= overflows_required)  // If required number of overflows has occurred
    {
        sirenState = !sirenState; // Toggle siren state
        overflow_count = 0;  // Reset overflow count
    }

    
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Clear overflow flag
}

/** CODE: Write your code for Ex E5.0 above this line. */


//MSP430FR6989
//Displays value from ADC

#include "msp430.h"
#include <stdlib.h>


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    /* P1.0 Signal that turns fan on/off.*/
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    P1OUT &= ~BIT0;                         // Switch LED off
    TA0CCTL1 = OUTMOD_7;                    //Set/Reset
    TA0CTL = TASSEL_2 + MC_1 +TACLR ;
    TA0CCR0 = 100;
    TA0CCR1 = 50;
    P1SEL0 |= BIT0;                         // PWM output to LED P1.0
    P1SEL1 &= ~BIT0;


    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    /*UART Initialization*/
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation GREEN 2.1 WHITE 2.0
    P2SEL1 &= ~(BIT0 | BIT1);

    /* set timer for UART*/
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt



    __enable_interrupt();
    __bis_SR_register(GIE);
    //adc
    while (1)
    {
     // For debugger
    }

}


#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        while(!(UCA0IFG & UCTXIFG)); // Wait until buffer is ready
        UCA0TXBUF = UCA0RXBUF;
        TA0CCR1 = UCA0RXBUF;
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}

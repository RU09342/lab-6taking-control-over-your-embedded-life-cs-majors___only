#include <msp430.h>

unsigned int adc_value =0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    //Configure the port to be used as ADC input

    //Pick P1.2, P1.3, P1.4, and P1.5 as output for most significant bits
    P1DIR = BIT2 + BIT3 + BIT4 + BIT5;
    P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);

    //Pick P2.4, and P2.5, as output
    P2DIR = BIT4 + BIT5;
    P2OUT &= ~(BIT4 + BIT5);

    //Pick P4.0, and P4.3, as output for least significant bits
    P4DIR = BIT0 + BIT3;
    P4OUT &= ~(BIT0 + BIT3);

    P3SEL |= BIT3; // UART TX
    P3SEL |= BIT4; // UART RX
    UCA0CTL1 |= UCSWRST; // Resets state machine
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 6; // 9600 Baud Rate
    UCA0BR1 = 0; // 9600 Baud Rate
    UCA0MCTL |= UCBRS_0; // Modulation
    UCA0MCTL |= UCBRF_13; // Modulation
    UCA0MCTL |= UCOS16; // Modulation
    UCA0CTL1 &= ~UCSWRST; // Initializes the state machine
    UCA0IE |= UCRXIE;


    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled
    __no_operation(); // For debugger
}

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{

    while (!(UCA0IFG&UCTXIFG)); // check if USCI_A0 TX buffer ready
    //Take the input number from UART
    char characterIN = UCA0RXBUF;
    UCA0TXBUF = UCA0RXBUF;
    // Create a fixed size array
    int output[8];
    int i;
    //Put each bit of the input buffer in to the appropriate output
    for (i = 0; i < 8; i++) {
        output[i] = (characterIN >> i) & 1;
    }
    if(output[0] == 1){
        P1OUT |= BIT2; //Most significant bit to P1.2
    }
    else{
        P1OUT &= ~BIT2; // Continue with the less significant bits
    }
    if(output[1] == 1){
        P1OUT |= BIT3;
    }
    else{
        P1OUT &= ~BIT3;
    }
    if(output[2] == 1){
        P1OUT |= BIT4;
    }
    else{
        P1OUT &= ~BIT4;
    }
    if(output[3] == 1){
        P1OUT |= BIT5;
    }
    else{
        P1OUT &= ~BIT5;
    }
    if(output[4] == 1){
        P2OUT |= BIT4;
    }
    else{
        P2OUT &= ~BIT4;
    }
    if(output[5] == 1){
        P2OUT |= BIT5;
    }
    else{
        P2OUT &= ~BIT5;
    }
    if(output[6] == 1){
        P4OUT |= BIT0;
    }
    else{
        P4OUT &= ~BIT0;
    }
    if(output[7] == 1){
        P4OUT |= BIT3; // Least significant bit to P4.3
    }
    else{
        P4OUT &= ~BIT3;
    }




}

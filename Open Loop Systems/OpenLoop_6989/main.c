//MSP430FR6989
//Displays value from ADC

#include "msp430.h"
#include <LCDDriver.h>
#include <stdlib.h>

void LCDInit();
char convertToChar(int);

int count = 0;
int compareADC = 0;
int currentTemp = 0;
int temp = 320;
int adc_value = 0;
int adcValue[3];


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
    //initialize lcd
    LCDInit();
    showChar('0',0);
    showChar('0',1);
    showChar('0',2);
    showChar('0',3);
    showChar('0',4);
    showChar('0',5);
    showChar('C',6);
    /**
     * Initialization inspired by TI resource center.
     */

    /* P1.0 Signal that turns fan on/off.*/
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    P1OUT &= ~BIT0;                         // Switch LED off

    /*Setting up timer for PWM initial DUTY cycle 50%.*/
    TA0CCTL1 = OUTMOD_7;
    TA0CTL = TASSEL_2 + MC_1 +TACLR ;
    TA0CCR0 = 1000;
    TA0CCR1 = 500;
    P1SEL0 |= BIT0;                         // PWM output to LED P1.0
    P1SEL1 &= ~BIT0;


    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC12
    REFCTL0=REFON + REFVSEL_2;           //Use a reference voltage of 2.5V
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;   // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                // Use sampling timer, ADC12SC conversion start, clock=OSC
    ADC12CTL2 |= ADC12RES_2;             // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_3;           // ADC input select
    ADC12IER0 |= ADC12IE0;               // Enable ADC conv complete interrupt
    ADC12CTL0 |= ADC12ENC;               //Enable conversion
    P1DIR &= ~BIT3;
    P1OUT &= ~BIT3;
    P1SEL0 |= BIT3;                      //Configure pin 1.3 for input channel 3
    P1SEL1 |= BIT3;

    /*UART pin Initialize, set to not GPIO*/
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation GREEN 2.1 WHITE 2.0
    P2SEL1 &= ~(BIT0 | BIT1);

    /*UART timer initialization*/
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
    //adc Tells MSP to read the ADC.
    while (1)
    {
        __delay_cycles(1000);
        ADC12CTL0 |= ADC12ENC | ADC12SC;    // Start sampling/conversion
        __bis_SR_register(LPM0_bits | GIE); // LPM0, ADC12_ISR will force exit
        __no_operation();                   // For debugger
    }

}



/* ADC Interrupt inspired by TI*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
    case ADC12IV_NONE:        break;    // Vector  0:  No interrupt
    case ADC12IV_ADC12OVIFG:  break;    // Vector  2:  ADC12MEMx Overflow
    case ADC12IV_ADC12TOVIFG: break;    // Vector  4:  Conversion time overflow
    case ADC12IV_ADC12HIIFG:  break;    // Vector  6:  ADC12BHI
    case ADC12IV_ADC12LOIFG:  break;    // Vector  8:  ADC12BLO
    case ADC12IV_ADC12INIFG:  break;    // Vector 10:  ADC12BIN
    case ADC12IV_ADC12IFG0:             // Vector 12:  ADC12MEM0 Interrupt
        /*Sets these values to the actual temperature derived from the ADC*/
        currentTemp = ((ADC12MEM0*3.35)/4096)*1000; //changes duty cycle
        adc_value = ((ADC12MEM0*3.35)/4096)*1000;

        //currentTemp = currentTemp - ((currentTemp - 320) / 8);
        //adc_value =  currentTemp - ((currentTemp - 320) / 8);

        /* Sets the FAN PWM to reach target value and stay steady.*/
        if(currentTemp > temp + 3)
        {
            TA0CCR1 = 1000;
        }
        else if(currentTemp < temp - 3)
        {
            TA0CCR1 = 0;
        }
        else if(temp >= 380 && temp < 390)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 40.5)/0.03) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 40.5)/0.03) * 10) ;
            }
        }
        else if(temp >= 390 && temp <= 395)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 41.5)/0.05) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 41.5)/0.05) * 10) ;
            }
        }
        else if(temp >= 395 && temp <= 419)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 44.3)/0.12) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 44.3)/0.12) * 10) ;
            }
        }
        else if(temp >= 419 && temp <= 467)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 51.5)/0.48) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 51.5)/0.48) * 10) ;
            }
        }
        else if(temp >= 467 && temp <= 483)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 62.7)/1.6) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 62.7)/1.6) * 10) ;
            }
        }
        else if(temp >= 483 && temp <= 489)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 53.7)/.6) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 53.7)/.6) * 10) ;
            }
        }
        else if(temp >= 489 && temp <= 505)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 61.7)/1.6) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 61.7)/1.6) * 10) ;
            }
        }
        else if(temp >= 505 && temp <= 554)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 84.8)/4.9) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 84.8)/4.9) * 10) ;
            }
        }
        else if(temp >= 554 && temp <= 655)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 116)/10.1) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 116)/10.1) * 10) ;
            }
        }
        else if(temp >= 655)
        {
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = (-1 * (((temp/10) - 70.9)/1.08) * 10) ;
            }
            else if( currentTemp >= temp - 3 && currentTemp <= temp)
            {
                TA0CCR1 = (-1 * (((temp/10) - 70.9)/1.08) * 10) ;
            }
        }
        else{
            if(currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = 200 + ((currentTemp - temp) * 85 + (currentTemp - temp));
            }
            else if( currentTemp >= temp && currentTemp <= temp + 3)
            {
                TA0CCR1 = 200 + ((currentTemp - temp) * 6  - (currentTemp - temp));
            }
        }

        //TA0CCR1 = TA0CCR1/10;


        count = 0;

        //iterates through 3 digit adc_value and puts each digit into adcValue in reverse order
        do{
            adcValue[count]=(adc_value%10);
            adc_value/=10;
            count++;
        }while(adc_value>0);


        //prints out characters to lcd screen
        showChar(convertToChar(adcValue[2]), 3);
        showChar(convertToChar(adcValue[1]), 4);
        showChar(convertToChar(adcValue[0]), 5);




        // Exit from LPM0 and continue executing main
        __bic_SR_register_on_exit(LPM0_bits);
        break;
    case ADC12IV_ADC12IFG1:   break;    // Vector 14:  ADC12MEM1
    case ADC12IV_ADC12IFG2:   break;    // Vector 16:  ADC12MEM2
    case ADC12IV_ADC12IFG3:   break;    // Vector 18:  ADC12MEM3
    case ADC12IV_ADC12IFG4:   break;    // Vector 20:  ADC12MEM4
    case ADC12IV_ADC12IFG5:   break;    // Vector 22:  ADC12MEM5
    case ADC12IV_ADC12IFG6:   break;    // Vector 24:  ADC12MEM6
    case ADC12IV_ADC12IFG7:   break;    // Vector 26:  ADC12MEM7
    case ADC12IV_ADC12IFG8:   break;    // Vector 28:  ADC12MEM8
    case ADC12IV_ADC12IFG9:   break;    // Vector 30:  ADC12MEM9
    case ADC12IV_ADC12IFG10:  break;    // Vector 32:  ADC12MEM10
    case ADC12IV_ADC12IFG11:  break;    // Vector 34:  ADC12MEM11
    case ADC12IV_ADC12IFG12:  break;    // Vector 36:  ADC12MEM12
    case ADC12IV_ADC12IFG13:  break;    // Vector 38:  ADC12MEM13
    case ADC12IV_ADC12IFG14:  break;    // Vector 40:  ADC12MEM14
    case ADC12IV_ADC12IFG15:  break;    // Vector 42:  ADC12MEM15
    case ADC12IV_ADC12IFG16:  break;    // Vector 44:  ADC12MEM16
    case ADC12IV_ADC12IFG17:  break;    // Vector 46:  ADC12MEM17
    case ADC12IV_ADC12IFG18:  break;    // Vector 48:  ADC12MEM18
    case ADC12IV_ADC12IFG19:  break;    // Vector 50:  ADC12MEM19
    case ADC12IV_ADC12IFG20:  break;    // Vector 52:  ADC12MEM20
    case ADC12IV_ADC12IFG21:  break;    // Vector 54:  ADC12MEM21
    case ADC12IV_ADC12IFG22:  break;    // Vector 56:  ADC12MEM22
    case ADC12IV_ADC12IFG23:  break;    // Vector 58:  ADC12MEM23
    case ADC12IV_ADC12IFG24:  break;    // Vector 60:  ADC12MEM24
    case ADC12IV_ADC12IFG25:  break;    // Vector 62:  ADC12MEM25
    case ADC12IV_ADC12IFG26:  break;    // Vector 64:  ADC12MEM26
    case ADC12IV_ADC12IFG27:  break;    // Vector 66:  ADC12MEM27
    case ADC12IV_ADC12IFG28:  break;    // Vector 68:  ADC12MEM28
    case ADC12IV_ADC12IFG29:  break;    // Vector 70:  ADC12MEM29
    case ADC12IV_ADC12IFG30:  break;    // Vector 72:  ADC12MEM30
    case ADC12IV_ADC12IFG31:  break;    // Vector 74:  ADC12MEM31
    case ADC12IV_ADC12RDYIFG: break;    // Vector 76:  ADC12RDY
    default: break;
    }
}

//inits lcd
void LCDInit()
{
    PJSEL0 = BIT4 | BIT5;                   // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do
    {
        CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory
    //Turn LCD on
    LCDCCTL0 |= LCDON;
}

/* converts an int number into a character to be displayed on the LCD Screen*/
char convertToChar(int digitIn){
    char digit;

    switch(digitIn){
    case 0:{
        digit = '0';
        break;
    }
    case 1:{
        digit = '1';
        break;
    }
    case 2:{
        digit = '2';
        break;
    }
    case 3:{
        digit = '3';
        break;
    }
    case 4:{
        digit = '4';
        break;
    }
    case 5:{
        digit = '5';
        break;
    }
    case 6:{
        digit = '6';
        break;
    }
    case 7:{
        digit = '7';
        break;
    }
    case 8:{
        digit = '8';
        break;
    }
    case 9:{
        digit = '9';
        break;
    }

    }

    return digit;



}

/*UART Interrupt*/
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        while(!(UCA0IFG & UCTXIFG)); // Wait until buffer is ready
        UCA0TXBUF = UCA0RXBUF;
        /*Sets the target temperature to a user specified value, multiply by 10 to keep it consistent with current temperature readings.*/
        temp = UCA0RXBUF*10;
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}

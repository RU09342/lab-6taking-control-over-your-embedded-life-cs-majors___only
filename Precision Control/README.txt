# Lab 6 Precision Control Austin Huang and Matthew Rodriguez

# PWM Part 2
A circuit was built to handle the PWM wave input from the MSP430. The circuit designed was an integrator to handle the AC wave. 

The output of the MSP430 was set to be a square wave. Because the input (Figure 12) to the circuit from the MSP430 was a square wave, 
the integration expected would be a triangle wave circuit from the MSP430 was a square wave, the integration expected would be a triangle 
wave. 

## R2R DAC
A circuit was designed to convert the digital output of the MSP430 to an analog DC signal The theoretical output was expected to be 1.475V 
given an input of 01110010 (114) from the MSP430. 

Each resistor in an R2R ladder will split an incoming voltage by some power of 2. The resistor farthest from your output will be controlled 
by the least significant bit, and will contribute the least voltage if on. (if off, it will contribute nothing.) The resistor closest to 
your output will be controlled by the most significant bit, and will contribute the most voltage if on. The specific amount of voltage 
each resistor contributes is the total voltage divided by 2 to the power of the bit’s place in the binary number from left to right. The 
most significant bit has a place of 1, the second most 2, etc. In an 8 bit DAC R2R convertor, the most significant bit will contribute 
half the voltage if on, (total voltage / 2^1) and the least significant bit will contribute the total voltage divided by 2^8, or 1/256 
of the total voltage. Thus, our input of 01110010 was expected to be 57/128 of the total voltage. (1/128 + 8/128 + 16/128+ 32/128) The 
least significant on bit, having a place of 7 in terms of significance, was used as the reference. (1/2^7)


## Loading Effects
A model in TINA-TI was built to analyze the loading effect of the boards except the input of the circuit was created 
by the waveform generator instead of the MSP430. The input of the circuit was set to a 50mV PP square wave. The circuit was then tested 
with varying resistor values 100Ohm, 1kOhm, 270kOhm, 840kOhm, and 2MOhm as the load to observe the loading effect. Initially, the test did not 
involve a load. This produced a triangle wave, since the circuit involved integrating a square wave. 

The output without the load was used as a reference to see the effects of bigger and bigger loads on the circuit output. The output voltage
of very large loads is significantly smaller than the original output with a small or negligible load.

This experimentation allows for consideration for higher level design. Considering the loading effect of very large or very small loads 
is an integral part of circuit design. 

NOTE: This README is only a brief synopsis of the process used to collect and analyze data. Attached is a Word Document with additional details on the Precision 
Control aspect of the lab.
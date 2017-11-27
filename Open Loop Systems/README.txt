# Open Loop Systems
## Fan Control
A fan is used to regulate the temperature of the voltage regulator. The fan was used to see if the signal from the MSP430 passing 
through the circuit would able to power the fan. The MSP430 is used to output a PWM signal to the fan. The circuit in Figure 1 was built and tested to power the fan.  

## Voltage Regulator and Temperature Reading
The output voltage of the temperature sensor determines the temperature it is reading. The calibration ratio is seen to be 10mV per 
degree Celsius. Using the given ratio, any output voltage measured can be converted to a corresponding temperature in degrees Celsius. 

A 5V voltage regulator was used to drop a 15V-20V input. The range was chosen to be a high voltage for the regulator to heat up. This 
would allow for testing the functionality of the temperature sensor. A simple circuit was built to test the temperature sensor.


The result will be recorded in a Voltage vs. Time graph. The voltage is exptected to increase over 
time as the temperature sensor reads the temperature of the voltage regulator.


## System Modeling
A system can be modeled for the temperature and duty cycle of the fan. Temperature values were sampled at different PWM duty cycles 
emitted by the MSP430. From the recorded data, a graph was constructed to model the relationship of the duty cycle of the fan
to the temperature of the regulator. 

## Open Loop Control System
A control system can be designed from a coding standpoint given the system model of the voltage regulator. Since the data models the 
relationship between the duty cycle of the fan to a temperature, the behavior of the output of the MSP430 can be coded. The PWM duty 
cycle output will vary according to a desired temperature inputted. The code is attached in the repository. 

## Code

NOTE: This README is only a brief synopsis of the process used to collect and analyze data. Attached is a Word Document with additional details on the Open 
Loop Systems aspect of the lab.
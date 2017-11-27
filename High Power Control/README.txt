# High Power Control
Austin Huang and Matthew Rodriguez

## MOSFET Switching
The MOSFET switch is a very simple circuit which can be used in a multitude of applications. One of the most important features 
of the MOSFET Switch is the near zero current it takes to switch the MOSFET from an on to an off state. There are two main architectures, 
low-side and high-side switch, each requiring a different type of MOSFET.

A simple circuit was built and tested to observe the functionality of the MOSFET. The MOSFET was used to handle a PWM wave from the MSP430
to power a fan. 

The circuit was tested again with an input from the wave form generator instead of the MSP430. This would allow for free and easy 
adjustment of the input frequency to test the switching speed of the MOSFET. The frequency was raised until “weird behavior” occurred
which determined the limitation of the switching speed of the MOSFET.

## Relays
A relay is an electro-mechanical system which can open and close a switch based on an input. These are extremely useful in situations 
where large amounts of current need to flow, such as in automotive applications, but they do have their limits. For starters, since the
actuation process requires a constant current, sometimes this can be too much for the processor to handle. 

The relay essentially acts as a mechanical switch. Upon studying the pin out of the relay, a circuit was built to test the functionality 
and limitations of a relay. By using a constant 12V DC signal across the coil of the relay and a 12V DC input, a fan was able to be 
powered. The relay acted as a switch, where the excitement of the electromagnetic coil pulled up the switch causing a connection to 
be made allowing the fan to receive the voltage required to turn on.

Similarly, the circuit was tested with an AC signal across the coil as well. The relay was expected to switch back and forth between 
the normally open and normally closed pins allowing for the fan to be powered by a voltage mimicking a PWM signal. The results are 
confirmed in the attached video to this repository. The relay’s switching mechanic is quite audible as the connection is made to power 
the fan. Clicks in rapid succession are heard as the switch goes back and forth. The fan was successfully powered as seen in the video.
 
The limitations of the relay were also tested. Since it is a mechanical device, it is not expected to handle very high frequencies. 
The frequency of the AC signal across the coil was increased until a breaking point or cut-off frequency for the device was found. 
The breaking point was defined to be dependent on its audible reactions to the increased frequency. Once the relay was observed to be 
failing from audio ques, the cut-off frequency was recorded. The result was estimated to be around the 200 Hz range. 

NOTE: This README is only a brief synopsis of the process used to collect and analyze data. Attached is a Word Document with additional 
details on the High Power Control aspect of the lab.

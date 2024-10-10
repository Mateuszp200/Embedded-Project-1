# Embedded-Project-1
Project Target:
Write a single interrupt-driven program to do the following tasks concurrently:
1.	Use the connected output port bits of an ATmega4809 microcontroller that are used on the Arduino Nano Every to implement a travelling single LED pattern (cylon eyes) on the LED shield provided with your Arduino board. The LED array is connected to the microcontroller’s output ports as shown in the Appendix. The on/off time of the LED must be set using a timer interrupt, where the on/off time is based on the input voltage applied to ADC input AIN3 of the microcontroller. You must not use a delay loop for on/off timing and you must write to the LED array in the Timer ISR.
2.	Use the LED array to provide a graphical display of the input voltage applied to the ADC0 input 3 (AIN3) of the microcontroller in ‘thermometer’ format.
3.	Read the input switch attached to PORTE bit 1, and use the value read to select between the use of LED array for a cylon eyes type display or the ADC value thermometer display. When the switch is NOT pressed (reads HIGH), display the cylon pattern; when the switch IS pressed (reads LOW), display the ADC value described in Point 2.

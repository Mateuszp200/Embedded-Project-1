
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define PORT_INT0LVL_LO_gc 0x01

#define LED_COUNT 10
#define POTENTIOMETER_PIN 3 // AIN[3], connected to potentiometer
#define BUTTON_PIN 1 // PORTE bit 1 (PE1)
#define CYLON_MODE 0
#define THERMOMETER_MODE 1

uint16_t potentiometer_value = 0;
uint8_t thermometer_level = 0;
volatile uint8_t mode = CYLON_MODE; // Start with cylon mode

// Function to initialize ADC for potentiometer reading
void ADC_init() {
	// Configure ADC for single-ended mode and enable ADC
	ADC0.CTRLA = ADC_ENABLE_bm;
	// Set reference voltage to VCC and configure resolution
	ADC0.CTRLB = ADC_RESSEL_10BIT_gc;
	// Set ADC prescaler to achieve a suitable conversion rate
	ADC0.CTRLC = ADC_PRESC_DIV64_gc;
	// Set ADC input to potentiometer pin
	ADC0.MUXPOS = POTENTIOMETER_PIN;
}

// Function to read potentiometer value using ADC
uint16_t readPotentiometer() {
	// Start ADC conversion
	ADC0.COMMAND = ADC_STCONV_bm;
	// Wait for conversion to complete
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
	// Return ADC result
	return ADC0.RES;
}

// Function to initialize GPIO pins
void GPIO_init() {
	// Set PORTE bit 1 as input with pull-up resistor (for button)
	PORTE.DIRCLR = PIN1_bm;
	PORTE.PIN1CTRL = PORT_PULLUPEN_bm;
	// Set LED pins as outputs
	PORTC.DIR = PIN6_bm | PIN5_bm | PIN4_bm;  // PC4, PC5, PC6
	PORTA.DIR = PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm; // PA0, PA1, PA2, PA3
	PORTB.DIR = PIN2_bm; // PB2
	PORTF.DIR = PIN5_bm | PIN4_bm; // PF4, PF5
}

// Function to update thermometer display
void updateThermometer() {
	// Calculate thermometer level based on potentiometer value
	thermometer_level = (potentiometer_value * LED_COUNT) / 1024;

	// Turn off all LEDs
	PORTC.OUT &= ~(PIN6_bm | PIN5_bm | PIN4_bm); // Clear PC4, PC5, PC6
	PORTA.OUT &= ~(PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm); // Clear PA0, PA1, PA2, PA3
	PORTB.OUT &= ~(PIN2_bm); // Clear PB2
	PORTF.OUT &= ~(PIN5_bm | PIN4_bm); // Clear PF4, PF5

	// Turn on LEDs up to calculated level
	for (uint8_t i = 0; i < thermometer_level; i++) {
		switch (i) {
			case 0:
			PORTC.OUT |= PIN5_bm; // Set PC4 (LED 0)
			break;
			case 1:
			PORTC.OUT |= PIN4_bm; // Set PC5 (LED 1)
			break;
			case 2:
			PORTA.OUT |= PIN0_bm; // Set PA0 (LED 2)
			break;
			case 3:
			PORTF.OUT |= PIN5_bm; // Set PF5 (LED 3)
			break;
			case 4:
			PORTC.OUT |= PIN6_bm; // Set PC6 (LED 4)
			break;
			case 5:
			PORTB.OUT |= PIN2_bm; // Set PB2 (LED 5)
			break;
			case 6:
			PORTF.OUT |= PIN4_bm; // Set PF4 (LED 6)
			break;
			case 7:
			PORTA.OUT |= PIN1_bm; // Set PA1 (LED 7)
			break;
			case 8:
			PORTA.OUT |= PIN2_bm; // Set PA2 (LED 8)
			break;
			case 9:
			PORTA.OUT |= PIN3_bm; // Set PA3 (LED 9)
			break;
		}
	}
}

// Function to run cylon effect
void runCylonEffect(uint8_t delay_ms) {
	// Loop through the cylon effect
	for (uint8_t i = 0; i < LED_COUNT; i++) {
		ToggleLED(i, 1); // Turn on LED
		_delay_ms(1000);
		ToggleLED(i, 0); // Turn off LED
	}

	for (uint8_t i = LED_COUNT - 2; i > 0; i--) {
		ToggleLED(i, 1); // Turn on LED
		_delay_ms(1000);
		ToggleLED(i, 0); // Turn off LED
	}
}

// Function to toggle LEDs
void ToggleLED(uint8_t led, uint8_t on) {
	switch (led) {
		case 0:
		if (on) PORTC.OUT |= PIN5_bm; // Turn LED on
		else PORTC.OUT &= ~PIN5_bm;   // Turn LED off
		break;
		case 1:
		if (on) PORTC.OUT |= PIN4_bm;
		else PORTC.OUT &= ~PIN4_bm;
		break;
		case 2:
		if (on) PORTA.OUT |= PIN0_bm;
		else PORTA.OUT &= ~PIN0_bm;
		break;
		case 3:
		if (on) PORTF.OUT |= PIN5_bm;
		else PORTF.OUT &= ~PIN5_bm;
		break;
		case 4:
		if (on) PORTC.OUT |= PIN6_bm;
		else PORTC.OUT &= ~PIN6_bm;
		break;
		case 5:
		if (on) PORTB.OUT |= PIN2_bm;
		else PORTB.OUT &= ~PIN2_bm;
		break;
		case 6:
		if (on) PORTF.OUT |= PIN4_bm;
		else PORTF.OUT &= ~PIN4_bm;
		break;
		case 7:
		if (on) PORTA.OUT |= PIN1_bm;
		else PORTA.OUT &= ~PIN1_bm;
		break;
		case 8:
		if (on) PORTA.OUT |= PIN2_bm;
		else PORTA.OUT &= ~PIN2_bm;
		break;
		case 9:
		if (on) PORTA.OUT |= PIN3_bm;
		else PORTA.OUT &= ~PIN3_bm;
		break;
	}
}

ISR(RTC_PIT_vect)
{
	// Clear RTC PIT Timer interrupt flag
	RTC.PITINTFLAGS = RTC_PI_bm;

	// Toggle the state of the display mode
	mode = (mode == CYLON_MODE) ? THERMOMETER_MODE : CYLON_MODE;
}

// External Interrupt 0 handler for button press
ISR(PORTE_PORT_vect) {
	// Check if button pin caused the interrupt
	if (PORTE.INTFLAGS & PIN1_bm) {
		// Clear the interrupt flag
		PORTE.INTFLAGS = PIN1_bm;

		// Switch to thermometer mode
		mode = THERMOMETER_MODE;
	}
}

int main(void) {
	// Initialize peripherals
	ADC_init();
	GPIO_init();
	
	// Enable global interrupts
	sei();
	
	// Configure button pin for interrupt
	PORTE.PIN1CTRL = PORT_ISC_FALLING_gc; // Interrupt on falling edge

	
	while (1) {
		// Run mode
		if (mode == THERMOMETER_MODE) {
			// Read potentiometer value
			potentiometer_value = readPotentiometer();
			// Thermometer display
			updateThermometer();
			} else {
			// Run the cylon effect with a delay of 100 ms
			runCylonEffect(100);
		}
	}
	
	return 0;
}

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Variables to represent the seconds, minutes and hours
// For the stop watch
unsigned char g_second = 0;
unsigned char g_minute = 0;
unsigned char g_hour = 0;

void Timer1_Init(void) {
	TCCR1A = (1 << FOC1A) | (1 << FOC1B);
	// Setting the required bits to operate in CTC mode
	// With pre-scaler 256
	TCCR1B = (1 << WGM12) | (1 << CS12);

	// Initialize the count value with zero
	TCNT1 = 0;
	// Initialize the compare register to make
	// Compare match every one second
	OCR1A = 3907;

	// Enable the Interrupt for compare match A
	TIMSK |= (1 << OCIE1A);
}

void INT0_Init(void) {
	// Configure PD2 as input pin
	DDRD &= ~(1 << PD2);
	// Enable the internal pull up resistor
	PORTD |= (1 << PD2);
	// Enable the interrupt for INT0
	GICR |= (1 << INT0);
	// Configure the required bits to detect falling edge
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
}

void INT1_Init(void) {
	// Configure PD3 as input pin
	DDRD &= ~(1 << PD3);
	// Enable the interrupt for INT1
	GICR |= (1 << INT1);
	// Configure the required bits to detect rising edge
	MCUCR |= (1 << ISC11);
	MCUCR |= (1 << ISC10);
}

void INT2_Init(void) {
	// Configure PB2 as input pin
	DDRB &= ~(1 << PB2);
	// Enable the internal pull up resistor
	PORTB |= (1 << PB2);
	// Enable the interrupt for INT2
	GICR |= (1 << INT2);
	// Configure the required bit to detect falling edge
	MCUCSR &= ~(1 << ISC2);
}

// Handling the seconds, minutes and hours
ISR(TIMER1_COMPA_vect) {
	g_second++;
	if (g_second == 60) {
		g_second = 0;
		g_minute++;
		if (g_minute == 60) {
			g_minute = 0;
			g_hour++;
		}
	}
}

// Reset the stop watch
ISR(INT0_vect) {
	g_second = 0;
	g_minute = 0;
	g_hour = 0;

	TCNT1 = 0;
}

// Pause the stop watch
ISR(INT1_vect) {
	TCCR1B &= ~(1 << CS12);
}

// Resume the stop watch
ISR(INT2_vect) {
	TCCR1B |= (1 << CS12);
}

int main(void) {

	// Enable the global interrupt
	SREG |= (1 << 7);

	INT0_Init();
	INT1_Init();
	INT2_Init();
	Timer1_Init();

	// Configure the first four pins as output pins in PORTC
	// For the decoder 7447
	DDRC |= 0x0F;
	// Configure the first six pins as output pins in PORTA
	// To select the 7 segment to display
	DDRA |= 0x3F;

	// represent the digit to be displayed
	// On the selected 7 segment
	unsigned char digitToBeDisplayed = 0;

	while (1) {

		// Code to display on the 7 segments using
		// multiplexed technique

		// Display the first digit in g_second
		digitToBeDisplayed = g_second % 10;
		PORTA = (PORTA & 0xC0) | (0x01);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);

		// Display the second digit in g_second
		digitToBeDisplayed = g_second / 10;
		PORTA = (PORTA & 0xC0) | (0x02);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);

		// Display the first digit in g_minute
		digitToBeDisplayed = g_minute % 10;
		PORTA = (PORTA & 0xC0) | (0x04);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);

		// Display the second digit in g_minute
		digitToBeDisplayed = g_minute / 10;
		PORTA = (PORTA & 0xC0) | (0x08);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);

		// Display the first digit in g_hour
		digitToBeDisplayed = g_hour % 10;
		PORTA = (PORTA & 0xC0) | (0x10);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);

		// Display the second digit in g_hour
		digitToBeDisplayed = g_hour / 10;
		PORTA = (PORTA & 0xC0) | (0x20);
		PORTC = (PORTC & 0xF0) | digitToBeDisplayed;
		_delay_ms(2);
	}

	return 0;
}

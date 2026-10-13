#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int toggle = 1;

void	init()
{
	// Enable output direction for the light D1
	DDRB = (1 << DDB0);

	// Enable pull up resistor for the SW1 switch
	PORTD = (1 << PORTD2);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// Pin Change Interrupt Enable 0 for the SW1 Switch
	EIMSK = (1 << INT0);

	// Setup falling edge mode
	EICRA = (1 << ISC01);
}


SIGNAL(INT0_vect)
{
	// Disable interrupt for the associated pin
	EIMSK &= ~(1 << INT0);
	if (toggle)
	{
		// Set rising edge mode
		EICRA |= (1 << ISC00);

		// Toggle the LED D1
		PORTB ^= (1 << PORTB0);
		toggle = 0;
	}
	else
	{
		// Reset to falling edge mode
		EICRA &= ~(1 << ISC00);
		toggle = 1;
	}
	_delay_ms(50);

	// Enable interrupt for the associated pin
	EIMSK |= (1 << INT0);

	// Reset the interrupt flag for debouncing
	EIFR |= (1 << INTF0);
}

int main()
{
	init();
	while (1)
	{
	
	}
}
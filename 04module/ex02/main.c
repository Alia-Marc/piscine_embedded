#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	init()
{
	// Enable output direction for the light D1 D2 D3 D4
	DDRB = (1 << DDB0) | (1 << DDB1) |(1 << DDB2) | (1 << DDB4);

	// Enable pull up resistor for the SW1 switch and SW2 switch
	PORTD = (1 << PORTD2) | (1 << PORTD4);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// When the PCIE2 bit is set (one) pin change interrupt 2 is
	// enabled. Any change on any enabled PCINT[23:16] pin will cause an interrupt. 
	PCICR = (1 << PCIE2);

	// Pin Change Interrupt Enable PCINT20 for the SW2 Switch
	PCMSK2 =  (1 << PCINT20);

	// Pin Change Interrupt Enable for the SW1 Switch
	EIMSK = (1 << INT0);

	// Setup falling edge mode
	EICRA = (1 << ISC01);
}


int 	toggle_SW1 = 1;
int		toggle_SW2 = 1;
uint8_t value = 0;

SIGNAL(INT0_vect)
{
	// Disable interrupt for the associated pin
	EIMSK &= ~(1 << INT0);
	if (toggle_SW1)
	{
		// Set rising edge mode
		EICRA |= (1 << ISC00);

		toggle_SW1 = 0;
		if (value < 15)
		{
			value++;
			PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
		}
	}
	else
	{
		// Reset to falling edge mode
		EICRA &= ~(1 << ISC00);
		toggle_SW1 = 1;
	}
	_delay_ms(50);

	// Enable interrupt for the associated pin
	EIMSK |= (1 << INT0);
	// Reset the interrupt flag for debouncing
	EIFR |= (1 << INTF0);
}

SIGNAL(PCINT2_vect)
{
	// Disable interrupt for the associated pin
	PCMSK2 &= ~(1 << PCINT20);
	if (toggle_SW2)
	{
		toggle_SW2 = 0;
		if (value > 0)
		{
			value--;
			PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);	
		}
	}
	else
		toggle_SW2 = 1;
	_delay_ms(50);

	// Enable interrupt for the associated pin
	PCMSK2 |= (1 << PCINT20);
	// Reset the interrupt flag for debouncing
	PCIFR |= (1 << PCIF2);
}

int main()
{
	init();
	while (1)
	{
	
	}
}
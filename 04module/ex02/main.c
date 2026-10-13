#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Check sur le PIND si le bit correspondant est actif ou non
int	is_pressed(int bit)
{
	return (!(PIND >> bit & 1));
}

void	init()
{
	// Enable output direction for the light D1 D2 D3 D4
	DDRB = (1 << DDB0) | (1 << DDB1) |(1 << DDB2) | (1 << DDB4);

	// Enable pull up resistor for the SW1 switch and SW2 switch
	PORTD = (1 << PORTD2) | (1 << PORTD4);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	PCICR = (1 << PCIE2);

	// Pin Change Interrupt Enable 0 for the SW1 Switch
	PCMSK2 =  (1 << PCINT20);


	// Pin Change Interrupt Enable 0 for the SW1 Switch
	EIMSK = (1 << INT0);

	// Setup falling edge mode
	EICRA = (1 << ISC01);

	// // Setup timer1 with CTC mode on OCR1A but doesn't start it now
	// TCCR1B = (1 << WGM12);
	// Setup the top with 50ms delay with a 1024 prescaler
	// OCR1A = F_CPU / 1024 / 20;

	// // Set OCIE1A bit to 1 for the output compare A match Interrupt enable.
	// // So when TCNT1 reaches the TOP, the interrupt signal is sent
	// TIMSK1 = (1 << OCIE1A);
	// Each PCINT[23:16]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin. 
	// For our SW1 Switch, he's on the pin 18, by counting from PORTB to PORTD8
	
}

// SIGNAL(TIMER1_COMPA_vect)
// {
// 	// Disable timer1
// 	TCCR1B &= ~(1 << CS12 | 1 << CS10);
// 	TCNT1 = 0;

// 	// Enable interrupt for the associated pin
// 	PCMSK2 = (1 << PCINT18);
// 	PCIFR = (1 << PCIF2);
// }

int 	toggle_SW1 = 1;
int		toggle_SW2 = 1;
uint8_t value = 0;

char	last_state;
char	current_state;

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


// SIGNAL(PCINT2_vect)
// {
// 	current_state = is_pressed(PIND2) << 0 | is_pressed(PIND4) << 1;

// 	PCMSK2 &= ~(1 << PCINT18);
// 	// Disable interrupt for the associated pin
// 	if (!(last_state & 1) && (current_state & 1) && toggle)
// 	{
// 		value++;
// 		toggle = 0;
// 		value = value % 16;
// 		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
// 		_delay_ms(50);
// 	}
// 	else
// 		toggle = 1;

// 	PCMSK2 |= (1 << PCINT18);
// 	// }
// 	// else if (!(last_state >> 1 & 1) && (current_state >> 1 & 1))
// 	// {
// 	// 	PCMSK2 &= ~(1 << PCINT20);
// 	// 	if (toggle)
// 	// 	{
// 	// 		value++;
// 	// 		toggle2 = 0;
// 	// 		value = value % 16;
// 	// 		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
	
// 	// 	}
// 	// 	else
// 	// 	{
// 	// 		toggle2 = 1;
// 	// 		PCMSK2 |= (1 << PCINT20);
// 	// 	}
// 	// }

// 	// Assigne la value a chaque PORTBx en fonction de la value

// 	// Enable interrupt for the associated pin

// 	// // Enable timer1 with CTC mode on OCR1A with a prescaler of 1024
// 	// TCCR1B |= (1 << CS12) | (1 << CS10);
// }

// SIGNAL(INT0_vect)
// {
// 	if (is_pressed(PIND2) && value < 15)
// 	{
// 		value++;
// 		//value = value % 16;
// 		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
// 		while (is_pressed(PIND2)){}
// 	}
// }

// SIGNAL(PCINT2_vect)
// {
// 	if (is_pressed(PIND4) && value > 0)
// 	{
// 		value--;
// 		//value = value % 16;
// 		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
// 		while (is_pressed(PIND4)){}
// 	}
// }

int main()
{
	init();
	while (1)
	{
	
	}
}
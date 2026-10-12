#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	init()
{
	// Enable output direction for the light D2
	DDRB = (1 << DDB1);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// Setup timer1 in pahse correct PWM mode 8bit with ICR1 on top, no prescaler
	TCCR1A = (1 << WGM10) | (1 << COM1A1);
	TCCR1B = (1 << CS10);

	OCR1A = 0;

	// Setup timer0 in CTC mode with OCR0A on top with a prescaling of 1024
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS02) | (1 << CS00);
	// F_CPU / 1024 / 510
	OCR0A = 31;

	// Set OCIE0A bit to 1 for the output compare A match Interrupt enable.
	// So when TCNT0 reaches the TOP, the interrupt signal is sent
	TIMSK0 = (1 << OCIE0A);
	
}

SIGNAL(TIMER0_COMPA_vect)
{
	static int8_t direction = 1;

	if (OCR1A == 255)
		direction = -1;
	else if (OCR1A == 0)
		direction = 1;
	OCR1A += direction;

}

int main()
{
	init();
	while (1)
	{
	
	}
}
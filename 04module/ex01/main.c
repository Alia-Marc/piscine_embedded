#include <avr/io.h>
#include <avr/interrupt.h>

#  define FT_SIGNAL(vector)					\
    void vector (void) __attribute__ ((__signal__, __INTR_ATTRS));		\
    void vector (void)


void	init()
{
	// Enable output direction for the light D2
	DDRB = (1 << DDB1);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// Activate clk with prescaling 256
	// Activate mode 10 PWM phase correct mode with ICR1 on TOP value
	TCCR1B |= (1 << CS12) | (1 << WGM13);
	TCCR1A |= (1 << WGM11) | (1 << COM1A1);

	ICR1 = 250;
	OCR1A = 0;

	// Setup timer0 in CTC mode with OCR0A on top with a prescaling of 256
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS02);

	// F_CPU / 256 / 500
	OCR0A = F_CPU / 256 / 500;

	// Set OCIE0A bit to 1 for the output compare A match Interrupt enable.
	// So when TCNT0 reaches the TOP, the interrupt signal is sent
	TIMSK0 = (1 << OCIE0A);
	
}

FT_SIGNAL(TIMER0_COMPA_vect)
{
	static int8_t direction = 1;

	if (OCR1A == 250)
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
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t pos = 0;

void init_rgb()
{
	// Enable output on all the R G B lights
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << 7);

	// Enable timer1 with CTC mode on OCR1A with a prescaler of 1024
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	OCR1A = 255;

	// Set OCIE1A bit to 1 for the output compare A match Interrupt enable.
	// So when TCNT1 reaches the TOP, the interrupt signal is sent
	TIMSK1 = (1 << OCIE1A);

	// Set timer0 to PWM phase correct mode with no prescaling
	// It clear and toggle OC0B and OC0A, for LED_R and LED_G
	TCCR0A = (1 << WGM00) | (1 << COM0B1) | (1 << COM0A1);
	TCCR0B = (1 << CS00);

	// Set timer2 to PWM phase correct mode with no prescaling
	// It clear and toggle OC2B for LED_B
	TCCR2A = (1 << WGM20) | (1 << COM2B1);
	TCCR2B = (1 << CS20);

}


void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	// Set la couleur respective en fonction de OC0B sur la led red, OC0A sur le green et OC2B sur le blue
	// We can see which one to toggle and how to use it in the chip schematic and then which timer it correspond
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}


void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		set_rgb(255 - pos * 3, 0, pos * 3);
	else if (pos < 170) 
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} 
	else 
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

SIGNAL(TIMER1_COMPA_vect)
{
	if (pos == 255)
		pos = 0;
	wheel(pos);
	pos++;
}

int main()
{
	init_rgb();
	while (1)
	{

	}
}
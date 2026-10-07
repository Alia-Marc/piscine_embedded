#include <avr/io.h>
#include <util/delay.h>

#define TOP (F_CPU / 256)

int	is_pressed(int bit)
{
	return (!(PIND >> bit & 1));
}

int main() 
{

	//OC1A connected to LED D2
	DDRB |= 1 << DDB1;

	// Activate clk with prescaling 256
	// Activate mode 14 fast PWM mode with ICR1 on TOP value
	TCCR1B |= (1 << CS12) | (1 << WGM12) |  (1 << WGM13);
	TCCR1A |= (1 << WGM11) | (1 << COM1A1);
	// An interrupt can be generated at each time the counter value
	// reaches the TOP value by either using the OFC1A or ICF1 flag
	// 16.1

	// In fast PWM mode 14, the counter is cleared to 0,
	// when the counter value TCNT1 matches  ICR1
	// The ICR1 define the TOP value for the counter

	ICR1 = TOP;
	OCR1A = TOP / 10;

	// Mets le port D sur les pins correspondants aux deux SW1 SW2 afin d'activer le pull up
	PORTD = (1 << PORTD2) | (1 << PORTD4);

	unsigned long 	time;
	char			last_state = is_pressed(PIND2) | is_pressed(PIND4) << 1;
	char			current_state;

	while (1)
	{


		time = OCR1A;
		// On regarde et assigne  l'etat actuel des deux SW1 SW2
		current_state = is_pressed(PIND2) << 0 | is_pressed(PIND4) << 1;

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on incremente time, en protegeant l'overflow
		if (!(last_state & 1) && (current_state & 1) && time < TOP)
				time += TOP / 10;

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on decremente time, en protegeant l'overflow
		if (!(last_state >> 1 & 1) && (current_state >> 1 & 1) && time > TOP / 10)
				time -= TOP / 10;
		
		last_state = current_state;
		OCR1A = time;
		_delay_ms(20);
	}
}

#include <avr/io.h>
#include <util/delay.h>


// Check sur le PIND si le bit correspondant est actif ou non
int	is_pressed(int bit)
{
	return (!(PIND >> bit & 1));
}

int main() 
{
	// Mets la Data Direction Register du port B en mode output sur les pins correspondants aux 4 leds
	DDRB = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);

	// Mets le port D sur les pins correspondants aux deux SW1 SW2 afin d'activer le pull up
	PORTD = (1 << PORTD2) | (1 << PORTD4);

	int 	value = 0;
	char	last_state = is_pressed(PIND2) | is_pressed(PIND4) << 1;
	char	current_state;

	while (1)
	{
		_delay_ms(50);

		// On regarde et assigne  l'etat actuel des deux SW1 SW2
		current_state = is_pressed(PIND2) << 0 | is_pressed(PIND4) << 1;

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on incremente la value, en protegeant l'overflow
		if (!(last_state & 1) && (current_state & 1) && value < 15)
			value++;

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on decremente la value, en protegeant l'overflow
		if (!(last_state >> 1 & 1) && (current_state >> 1 & 1) && value > 0)
			value--;

		last_state = current_state;

		// Assigne la value a chaque PORTBx en fonction de la value
		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
	}
}

// Informations utiles, sur la documentation de ATmega328p 14.1, 14.2, de comment utiliser les registres de chaque pin,
// en reperant lesquels utiliser sur le schematic de la chip
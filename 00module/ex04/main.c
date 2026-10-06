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
	DDRB = 1 << DDB0 | 1 << DDB1 | 1 << DDB2 | 1 << DDB4;

	// Mets le port D sur les pins correspondants aux deux SW1 SW2 afin d'activer le pull up
	PORTD = 1 << PORTD2 | 1 << PORTD4;

	int 	value = 0;
	char	SW1_last_state = is_pressed(PIND2);
	char	SW1_current_state;
	char	SW2_last_state = is_pressed(PIND4);
	char	SW2_current_state;

	while (1)
	{
		_delay_ms(50);

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on incremente la value, en protegeant l'overflow
		SW1_current_state = is_pressed(PIND2);
		if (!SW1_last_state && SW1_current_state && value < 15)
			value++;
		SW1_last_state = SW1_current_state;

		// Si l'etat actuel du PIND2 est actif et que l'etat precedant ne l'est plus, alors on decremente la value, en protegeant l'overflow
		SW2_current_state = is_pressed(PIND4);
		if (!SW2_last_state && SW2_current_state && value > 0)
			value--;
		SW2_last_state = SW2_current_state;

		// Assigne la value a chaque PORTBx en fonction de la value
		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
	}
}

// Informations utiles, sur la documentation de ATmega328p 14.1, 14.2, de comment utiliser les registres de chaque pin,
// en reperant lesquels utiliser sur le schematic de la chip
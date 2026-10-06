#include <avr/io.h>
#include <util/delay.h>


int	is_pressed(int bit)
{
	return (!((PIND >> bit) & 1));
}

int main() 
{
	DDRB = 1 << DDB0 | 1 << DDB1 | 1 << DDB2 | 1 << DDB4;

	int 	value = 0;
	char	SW1_pressed = is_pressed(PIND2);
	char	SW1_press;
	char	SW2_pressed = is_pressed(PIND4);
	char	SW2_press;
	while (1)
	{
		_delay_ms(50);

		SW1_press = is_pressed(PIND2);
		if (!SW1_pressed && SW1_press && value < 15)
			value++;
		SW1_pressed = SW1_press;

		SW2_press = is_pressed(PIND4);
		if (!SW2_pressed && SW2_press && value > 1)
			value--;
		SW2_pressed = SW2_press;
		
		value = value % 16;

		PORTB = ((1 << PORTB0) & value | (1 << PORTB1) & value | (1 << PORTB2) & value | ((1 << PORTB3) & value) << 1);
	
	}
}

// Informations utiles, sur la documentation de ATmega328p, de comment utiliser les registres de chaque pin,
// en reperant lesquels utiliser sur le schematic de la chip
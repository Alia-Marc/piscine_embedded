#include <avr/io.h>
#include <util/delay.h>

int main() 
{
	DDRB = 1 << DDB0;

	while (1)
	{
		// On check le troisieme bit du registery d'input du port D, qui est utilisee au SW1
		// Si le bit est 0 on toggle
		if (!((PIND >> PIND2) & 1))
		// On toggle le bit de PB0
			PORTB ^= (1 << PORTB0);
		// On attends que le bouton ne soit plus pressed avant de recheck a nouveau
		while (!((PIND >> PIND2) & 1)) {}
		// Petit delay pour eviter le bounce
		_delay_ms(50);
	}
}

// Informations utiles, sur la documentation de ATmega328p 14.1, 14.2, de comment utiliser les registres de chaque pin,
// en reperant lesquels utiliser sur le schematic de la chip
#include <avr/io.h>

int main() 
{
	DDRB = 1 << DDB0;

	while (1)
	{
		// On check le troisieme bit du registery d'input du port D, qui est utilisee au SW1
		// Si le bit est 0 on allume PB0 sinon on l'eteint
		if ((PIND >> PIND2) & 1)
		// On assigne 0 au premier bit de PORTB sans toucher aux autres
			PORTB &= ~(1 << PB0);
		else
		// On assigne 1 au premier bit de PORTB sans toucher aux autres
			PORTB |= (1 << PB0); 
	}
}

// Informations utiles, sur la documentation de ATmega328p 14.1, 14.2, de comment utiliser les registres de chaque pin,
// en reperant lesquels utiliser sur le schematic de la chip
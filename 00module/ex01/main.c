#include <avr/io.h>

int main() 
{
	// Mets la Data Direction Register du port B en mode output sur le pin correpondant
	DDRB = 1 << DDB0;
	
	// On change le premier bit de portB 00000000 -> 00000001 en donnant PB0
	// Qui est le pin associe a l'allumage de la led D1, sur le schematic de la chip
	// On peut donc allumer les led comme si c'etait une representatiom binaire, si je mets PORTB = 3 j'aurais la led D1 et D2 d'allumees
	// car 3 = 00000011
	PORTB = 1 << PB0;
}

// Informations utiles, sur la documentation de ATmega328p 14.1, 14.2, de comment utiliser les registres de chaque pin
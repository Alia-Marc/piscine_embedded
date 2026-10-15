#include <avr/io.h>
#include <avr/eeprom.h>

#include "uart.h"


uint8_t	EEPROM_read(unsigned int address)
{
	/* Wait for completion of previous write */
	while (EECR & (1 << EEPE))
		;
	/* Set up address register */
	EEAR = address;

	/* Start eeprom read by writing EERE */
	EECR |= (1 << EERE);

	/* Return data from Data Register */
	return (EEDR);
}


void	print_EEPROM_line(uint16_t *address)
{
	for (int i = 0; i < 16; i++)
	{
		uart_printhex8b(EEPROM_read(*address));
		uart_tx(' ');
		(*address)++;
	}
	uart_printstr("\n\r");
}

void	print_EEPROM(void)
{
	uint16_t	address = 0;
	for (int i = 0; i < 64; i++)
	{
		uart_printstr("00000");
		if (i < 10)
			uart_tx('0');
		uart_printnbr(i);
		uart_printstr("0 ");
		print_EEPROM_line(&address);
	}
}

int main()
{
	uart_init();
	print_EEPROM();
	while (1)
	{

	}
}
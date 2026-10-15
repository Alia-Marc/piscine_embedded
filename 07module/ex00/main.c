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
	char	line_buf[16];

	for (int i = 0; i < 16; i++)
	{
		line_buf[i] = EEPROM_read(*address);
		uart_printhex8b(line_buf[i]);
		uart_tx(' ');
		(*address)++;
	}
	uart_tx('|');
	for (int i = 0; i < 16; i++)
		uart_tx(is_printable(line_buf[i]) ? line_buf[i] : '.');
	uart_tx('|');
	uart_printstr("\n\r");
}

void	print_EEPROM(void)
{
	uint16_t	address = 0;
	for (int i = 0; i < 64; i++)
	{
		uart_printstr("00000");
		uart_printhex8b(i);
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
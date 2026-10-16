#include <avr/io.h>
#include <avr/eeprom.h>

#include "uart.h"

#define RED "\e[0;31m"
#define RESET "\033[0m"

uint16_t	hex_cast(uint16_t c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - '7');
	else if (c >= 'a' && c <= 'f')
		return (c - 'W');
}

uint16_t	ft_strlen(char *str)
{
	uint16_t	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

uint16_t	convert_hex_to_dec16b(char *hex_number)
{
	uint16_t	result;
	uint16_t	hex_len = ft_strlen(hex_number);
	uint16_t	shift = hex_len - 1;

	for (int i = 0; i < hex_len; i++)
	{
		result |= hex_cast(hex_number[i]) << shift * 4;
		shift--;
	}
	return (result);
}

uint8_t	EEPROM_read(uint16_t address)
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

void	EEPROM_write(uint16_t address, uint8_t data)
{
	/* Wait for completion of previous write */
	while(EECR & (1 << EEPE))
		;
	/* Set up address and Data Registers */
	EEAR = address;
	EEDR = data;
	/* Write logical one to EEMPE */
	EECR |= (1 << EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1 << EEPE);
}

void	print_EEPROM_line(uint16_t *address)
{
	uint8_t	line_buf[16];

	for (int i = 0; i < 16; i++)
	{
		line_buf[i] = EEPROM_read(*address);
		uart_printhex8b(line_buf[i]);
		(*address)++;
		i++;
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
	for (int i = 0; i < 4; i++)
	{
		uart_printstr("00000");
		uart_printhex8b(i);
		uart_printstr("0 ");
		print_EEPROM_line(&address);
	}
}

bool	is_already_there(uint16_t address, uint8_t data)
{
	uint8_t	read_byte = EEPROM_read(address);

	if (read_byte == data)
	{
		uart_printstr("it's there\n\r");
		return (1);
	}
	uart_printstr("it isn't there\n\r");
	return(0);
}

bool	is_hexadecimal(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

void	append_to_str(char *str, char c, int i)
{
	str[i] = c;
	str[i + 1] = '\0';
}

void	reset_buf(char *buf)
{
	for (int i = 0; i < 6; i++)
		buf[i] = '\0';
}

bool	parse_entry(char *buf, uint8_t length)
{
	uint8_t	first_byte_len = 0;
	uint8_t	second_byte_len = 0;
	bool	seen_space = false;

	for (int i = 0; i < length; i++)
	{
		if (buf[i] == ' ')
		{
			seen_space = true;
			continue ;
		}
		if (!seen_space)
			first_byte_len++;
		else
			second_byte_len++;
	}
	if ((first_byte_len < 1 || first_byte_len > 3) || second_byte_len != 2 || (first_byte_len == 3 && buf[0] > '3'))
		return (0);
	return (1);
}

bool	check_entry(char *buf, uint8_t length)
{
	uint8_t	seen_space = 0;

	for (int i = 0; i < 6; i++)
	{
		if (buf[i] == ' ')
			seen_space++;
	}
	if (seen_space > 1)
		return (0);
	return (parse_entry(buf, length));
}

void	loop(void)
{
	uint16_t	address;
	uint16_t	data;
	char		buf[7];
	uint8_t		length = 0;
	char		c;

	uart_tx('>');
	while (1)
	{
		c = uart_rx();
		if ((is_hexadecimal(c) || (c == ' ' && (length > 0 && length < 4))) && length < 6)
		{
			append_to_str(buf, c, length);
			length++;
			uart_tx(c);
		}
		else if (c == 127 && length > 0)
		{
			append_to_str(buf, '\0', length);
			length--;
			uart_tx('\b');
			uart_tx(' ');
			uart_tx('\b');
		}
		else if (c == 13)
		{
			uart_tx('\r');
			uart_tx('\n');
			if (check_entry(buf, length))
				uart_printstr("PARSE GOOD\n\r");
			else
				uart_printstr("Wrong format. Please use one of theses formats : x xx; xx xx; xxx xx. Max first arg: 3FF\n\r");
			//uart_printstr("WRONG ENTRY TYPE\n\r");
			reset_buf(buf);
			length = 0;
			uart_tx('>');
		}
	}
}

int main()
{
	uart_init();

	while (1)
	{
		loop();
	}
}
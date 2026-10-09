#include <avr/io.h>

void	uart_tx(char c);
char	uart_rx(void);
void 	set_rgb(uint8_t r, uint8_t g, uint8_t b);

void	append_to_str(char *str, char c, int i)
{
	str[i - 1] = c;
	str[i] = '\0';
}

void	uart_printstr(const char *str)
{
	int i = 0;

	while (str[i])
	{
		uart_tx(str[i]);
		i++;
	}
}

int	hex_cast(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - '7');
	else if (c >= 'a' && c <= 'f')
		return (c - 'W');
}

void	translate_hex(char *hex_number)
{
	uint8_t		r, g, b;

	r |= hex_cast(hex_number[1]) << 4;
	r |= hex_cast(hex_number[2]);
	g |= hex_cast(hex_number[3]) << 4;
	g |= hex_cast(hex_number[4]);
	b |= hex_cast(hex_number[5]) << 4;
	b |= hex_cast(hex_number[6]);
	set_rgb(r, g, b);

}

int		length = 0;
char	hex_number[8];

void	uart_rx_parse_hex_color()
{
	char	c = uart_rx();

	if (length == 0 && c == '#')
	{
		length++;
		append_to_str(hex_number, c, length);
		uart_tx(c);
	}
	else if ((c >= '0' && c <= '9' | c >= 'A' && c <= 'F' | c >= 'a' && c <= 'f') && (length > 0 && length < 7))
	{
		length++;
		append_to_str(hex_number, c, length);
		uart_tx(c);
	}
	else if (c == 127 && length > 0)
	{
		append_to_str(hex_number, '\0', length);
		length--;
		uart_tx('\b');
		uart_tx(' ');
		uart_tx('\b');
	}
	else if (c == 13 && length == 7)
	{
		uart_printstr("\t\t\tE");
		uart_printstr(hex_number);
		uart_tx('\n');
		uart_tx('\r');
		length = 0;
		
		translate_hex(hex_number);
	}
}

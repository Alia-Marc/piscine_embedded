#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200

#define USERNAME "Miaou"
#define PASSWORD "oui"

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	PRR &= ~(1 << PRUSART0);

	// MCU baud rate
	UBRR0L = F_CPU / (16 * BAUD);

	// Writing this TXEN0 bit to one enables the USART Transmitter. 20.11.3
	// Writing this RXEN0 bit to one enables the USART Receiver.
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0); 

	// Set both UMSELn1:0: bits to zero to select Asynchronous USART mode
	UCSR0C &= ~(1 << UMSEL00);
	UCSR0C &= ~(1 << UMSEL01);

	// Set USBSn to 0, so there is 1-bit number of stop bits to be inserted by the Transmitter.
	UCSR0C &= ~(1 << USBS0);

	// Set UCSZ01 and UCSZ00 to 1 to set the 8-bit character size
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

}

char	uart_rx(void)
{
	// Wait for data to be received
	while (!(UCSR0A & (1 << RXC0))) {}

	return (UDR0);

}

void	uart_tx(char c)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0))) {}

	// Put c into buffer, sends the data
	UDR0 = c;
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

void	display_auth(int step)
{
	if (!step)
		uart_printstr("Enter your login:\n\r\tusername: ");
	else
		uart_printstr("\tpassword: ");
}

void	append_to_str(char *str, char c, int i)
{
	str[i - 1] = c;
	str[i] = '\0';
}

int	ft_strcmp(char *s1, char *s2)
{
	int i = 0;

	while (s1[i]) 
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}
	if (s1[i] != s2[i])
		return (0);
	return (1);
}

int	check_creds(char *user_buf, char *pass_buf)
{
	if (ft_strcmp(USERNAME, user_buf) && ft_strcmp(PASSWORD, pass_buf))
	{
		uart_printstr("\n\rHello ");
		uart_printstr(USERNAME);
		uart_printstr("!\n\rShall we play a game?");
		return (-1);
	}
	uart_printstr("\n\rBad combination username/password\n\r\n\r");
	display_auth(0);
	return (1);
}

void	enter_creds()
{
	char	user_buf[19] = "";
	char	pass_buf[19] = "";
	int		length = 0;
	int		user = 1;
	char	c;

	while (user != -1)
	{
		c = uart_rx();
		if (c >= 32 && c <= 126 && length < 20)
		{
			length++;
			if (user)
			{
				append_to_str(user_buf, c, length);
				uart_tx(c);
			}
			else
			{
				append_to_str(pass_buf, c, length);
				uart_tx('*');
			}
		}
		else if (c == 127 && length > 0)
		{
			if (user)
				append_to_str(user_buf, '\0', length);
			else
				append_to_str(pass_buf, '\0', length);
			length--;
			uart_tx('\b');
			uart_tx(' ');
			uart_tx('\b');

		}
		else if (c == 13)
		{
			if (!user)
			{
				user = check_creds(user_buf, pass_buf);
				length = 0;
				continue;
			}
			uart_tx('\r');
			uart_tx('\n');
			display_auth(1);
			length = 0;
			user = 0;
		}
	}
}

int main() 
{
	uart_init();
	display_auth(0);
	enter_creds();
	DDRB = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
	while(1)
	{
		PORTB = (1 << PORTB0);
		_delay_ms(75);
		PORTB = (1 << PORTB1);
		_delay_ms(75);
		PORTB = (1 << PORTB2);
		_delay_ms(75);
		PORTB = (1 << PORTB4);
		_delay_ms(75);
	}
}

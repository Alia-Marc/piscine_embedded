#include <avr/io.h>

#define BAUD 115200

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

void	uart_printnbr(int n)
{
	if (n >= 0 && n < 10)
		uart_tx(n + 48);
	else if (n < 0)
	{
		uart_tx('-');
		uart_printnbr(n * (-1));
	}
	else
	{
		uart_printnbr(n / 10);
		uart_printnbr(n % 10);
	}
}

void uart_printhex8b(char c)
{
	uint8_t	n = c;
	int	n1 = n >> 4;
	int	n2 = n & 0x0F;
	if (n1 > 9)
		n1 += 7;
	uart_tx(n1 + '0');
	if (n2 > 9)
		n2 += 7;
	uart_tx(n2 + '0');
}

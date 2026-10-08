#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	//PRR &= ~(1 << PRUSART0);

	//unsigned int ubrr = F_CPU / (16 *(BAUD + 1));
	// MCU baud rate
	UBRR0L = 8;

	// Writing this TXEN0 bit to one enables the USART Transmitter. 20.11.3
	// Writing this RXEN0 bit to one enables the USART Receiver.
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); 

	// Set both UMSELn1:0: bits to zero to select Asynchronous USART mode
	//UCSR0C &= ~(1 << UMSEL00);
	//UCSR0C &= ~(1 << UMSEL01);

	// Set USBSn to 0, so there is 1-bit number of stop bits to be inserted by the Transmitter.
	//UCSR0C &= ~(1 << USBS0);

	// Set UCSZ01 and UCSZ00 to 1 to set the 8-bit character size
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

}

char	uart_rx(void)
{
	// Wait for data to be received

	while (!(UCSR0A & (1 << RXC0))) {}

	DDRB |= (1 << DDB1);
	PORTB ^= (1 << PORTB1);
	return (UDR0);

}

void	uart_tx(char c)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0))) {}

	// Put c into buffer, sends the data
	UDR0 = c;
}

int main() 
{
	DDRB |= (1 << DDB0);
	PORTB ^= (1 << PORTB0);
	uart_init();
	while(1)
	{
		uart_tx(uart_rx());
	}
}

// Send data
// void USART_Transmit( unsigned char data )
// {
// /* Wait for empty transmit buffer */
// while ( !( UCSRnA & (1<<UDREn)) )
// ;
// /* Put data into buffer, sends the data */
// UDRn = data;
// }

//Receive data
// unsigned char USART_Receive( void )
// {
// /* Wait for data to be received */
// while ( !(UCSRnA & (1<<RXCn)) )
// ;
// /* Get and return received data from buffer */
// return UDRn;
// }
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	PRR &= ~(1 << PRUSART0);

	// MCU baud rate
	UBRR0L = F_CPU / (16 * BAUD);

	// Writing this bit to one enables the USART Transmitter. 20.11.3
	UCSR0B |= (1 << TXEN0); 

	// Set both UMSELn1:0: bits to zero to select Asynchronous USART mode
	UCSR0C &= ~(1 << UMSEL00);
	UCSR0C &= ~(1 << UMSEL01);

	// Set USBSn to 0, so there is 1-bit number of stop bits to be inserted by the Transmitter.
	UCSR0C &= ~(1 << USBS0);

	// Set UCSZ01 and UCSZ00 to 1 t oset the 8-bit character size
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

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

	uart_init();
	while(1)
	{
		uart_tx('Z');
		_delay_ms(1000);
	}



}

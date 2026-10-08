#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200
#define TOP F_CPU / 1024 * 2

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	PRR &= ~(1 << PRUSART0);

	//unsigned int ubrr = F_CPU / (16 *(BAUD + 1));
	// MCU baud rate
	UBRR0L = 8;

	// Writing this bit to one enables the USART Transmitter. 20.11.3
	UCSR0B |= (1 << TXEN0); 

	// Set both UMSELn1:0: bits to zero to select Asynchronous USART mode
	UCSR0C &= ~(1 << UMSEL00);
	UCSR0C &= ~(1 << UMSEL01);

	// Set USBSn to 0, so there is 1-bit number of stop bits to be inserted by the Transmitter.
	UCSR0C &= ~(1 << USBS0);

	// Set UCSZ01 and UCSZ00 to 1 to set the 8-bit character size
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

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

// Setup the signal timer1_compA to do this function when the intterupt happens
SIGNAL(TIMER1_COMPA_vect)
{
	uart_printstr("Hello World!\n\r");
}

void	init_timer1()
{
	// Set clk with prescaling 1024
	// Activate CTC mode
	TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);

	// In CTC mode, the counter is cleared to 0,
	// when the counter value TCNT1 matches either the OCR1A or the ICR1A
	// The OCR1A define the TOP value for the counter
	OCR1A = TOP;

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << 7);

	// Set OCIE1A bit to 1 for the output compare A match Interrupt enable.
	// So when TCNT1 reaches the TOP, the interrupt signal is sent
	TIMSK1 |= (1 << OCIE1A);
}

int main() 
{
	uart_init();
	init_timer1();
	while(1)
	{

	}
}

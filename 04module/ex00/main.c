#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int	sw1_press = 0;
int	sw1_waspressed = 0;

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	PRR &= ~(1 << PRUSART0);

	// MCU baud rate
	UBRR0L = F_CPU / (16 * 115200);

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

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// Set RXCIE0 bit to one to enable the interrupt on receiving
	UCSR0B |= (1 << RXCIE0);
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

int toggle = 1;

void	init()
{
	// Enable output direction for the light D1
	DDRB = (1 << DDB0);

	// Enable pull up resistor for the SW1 switch
	PORTD = (1 << PORTD2);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << SREG_I);

	// Pin Change Interrupt Enable 0 for the SW1 Switch
	EIMSK = (1 << INT0);

	// Setup falling edge mode
	EICRA = (1 << ISC01);

	// // Setup timer1 with CTC mode on OCR1A but doesn't start it now
	// TCCR1B = (1 << WGM12);
	// Setup the top with 50ms delay with a 1024 prescaler
	// OCR1A = F_CPU / 1024 / 20;

	// // Set OCIE1A bit to 1 for the output compare A match Interrupt enable.
	// // So when TCNT1 reaches the TOP, the interrupt signal is sent
	// TIMSK1 = (1 << OCIE1A);
	// Each PCINT[23:16]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin. 
	// For our SW1 Switch, he's on the pin 18, by counting from PORTB to PORTD8
	
}

// SIGNAL(TIMER1_COMPA_vect)
// {
// 	// Disable timer1
// 	TCCR1B &= ~(1 << CS12 | 1 << CS10);
// 	TCNT1 = 0;

// 	// Enable interrupt for the associated pin
// 	PCMSK2 = (1 << PCINT18);
// 	PCIFR = (1 << PCIF2);
// }

SIGNAL(INT0_vect)
{
	// Disable interrupt for the associated pin
	EIMSK &= ~(1 << INT0);
	if (toggle)
	{
		// Set rising edge mode
		EICRA |= (1 << ISC00);

		// Toggle the LED D1
		PORTB ^= (1 << PORTB0);
		toggle = 0;
	}
	else
	{
		// Reset to falling edge mode
		EICRA &= ~(1 << ISC00);
		toggle = 1;
	}

	_delay_ms(60);

	// Enable interrupt for the associated pin
	EIMSK |= (1 << INT0);
	// // Enable timer1 with CTC mode on OCR1A with a prescaler of 1024
	// TCCR1B |= (1 << CS12) | (1 << CS10);
}

int main()
{
	init();
	uart_init();
	while (1)
	{
	
	}
}
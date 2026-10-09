#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_rx_parse_hex_color();

void	uart_init()
{
	// On assigne 0 au premier bit de PRR sans toucher aux autres afin de disable le power reduction de USART 
	PRR &= ~(1 << PRUSART0);

	//unsigned int ubrr = F_CPU / (16 *(BAUD + 1));
	// MCU baud rate
	UBRR0L = 8;

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
	SREG = (1 << 7);

	// Set RXCIE0 bit to one to enable the interrupt on receiving
	UCSR0B |= (1 << RXCIE0);
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

void init_rgb()
{
	// Enable output on all the R G B lights
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);

	// Set I-flag bit to 1 for interrupts to be enabled
	SREG = (1 << 7);
	
	// Set timer0 to PWM phase correct mode with no prescaling
	// It clear and toggle OC0B and OC0A, for LED_R and LED_G
	TCCR0A = (1 << WGM00) | (1 << COM0B1) | (1 << COM0A1);
	TCCR0B = (1 << CS00);

	// Set timer2 to PWM phase correct mode with no prescaling
	// It clear and toggle OC2B for LED_B
	TCCR2A = (1 << WGM20) | (1 << COM2B1);
	TCCR2B = (1 << CS20);

}


void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	// Set la couleur respective en fonction de OC0B sur la led red, OC0A sur le green et OC2B sur le blue
	// We can see which one to toggle and how to use it in the chip schematic and then which timer it correspond
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

SIGNAL(USART_RX_vect)
{
	uart_rx_parse_hex_color();
}

int main()
{
	uart_init();
	init_rgb();
	while (1)
	{

	}
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"

void	adc_init()
{
	// Disable power decution on ADC
	PRR &= ~(1 << PRADC);

	// Enable ADC, then setup prescaling to 64 to get 250khz from F_CPU / 64
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);


	// Multiplexer Selection Register
	// MUX[3:0]: Analog Channel Selection Bits 24-4
	// Choose the mod with only ADC0 enabled
	//ADMUX &= ~(1 << MUX3 | 1 << MUX2 | 1 << MUX1 | 1 << MUX0);

	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Then set ADLAR to one, to only have to read ADCH fwhen using only 8 bit
	ADMUX = (1 << REFS0) | (1 << ADLAR);
}

uint8_t	read_ADCH(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return (ADCH);
}

char	bin_to_hex(uint8_t small_byte)
{
	if (small_byte > 9)
		return (small_byte + 'a' - 10);
	return (small_byte + '0');

}

void	print_ADC0(void)
{
	uint8_t	byte = read_ADCH();

	uart_tx(bin_to_hex(byte >> 4));
	uart_tx(bin_to_hex(byte & 0x0F));
	uart_tx('\r');
	uart_tx('\n');
}

int main()
{
	adc_init();
	uart_init();
	while (1)
	{
		print_ADC0();
		_delay_ms(20);
	}
}
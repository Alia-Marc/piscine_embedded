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
	// Then set ADLAR to one, to only have to read ADCH when using only 8 bit
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

uint8_t	read_RV1(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Then set ADLAR to one, to only have to read ADCH when using only 8 bit
	// Set no MUX to choose readng from ADC0
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	return (read_ADCH());
}

uint8_t	read_LDR(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Then set ADLAR to one, to only have to read ADCH when using only 8 bit
	// Set no MUX0 to choose readng from ADC1, 24-4
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX0);
	return (read_ADCH());
}

uint8_t	read_NTC(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Then set ADLAR to one, to only have to read ADCH when using only 8 bit
	// Set MUX1 to choose readng from ADC2, 24-4
	ADMUX = (1 << REFS0) | (1 << ADLAR)| (1 << MUX1);
	return (read_ADCH());
}

void	read_and_print_all_potent(void)
{
	uint8_t	RV1_byte = read_RV1();
	uint8_t	LDR_byte = read_LDR();
	uint8_t	NTC_byte = read_NTC();

	uart_tx(bin_to_hex(RV1_byte >> 4));
	uart_tx(bin_to_hex(RV1_byte & 0x0F));
	uart_printstr(", ");
	uart_tx(bin_to_hex(LDR_byte >> 4));
	uart_tx(bin_to_hex(LDR_byte & 0x0F));
	uart_printstr(", ");
	uart_tx(bin_to_hex(NTC_byte >> 4));
	uart_tx(bin_to_hex(NTC_byte & 0x0F));
	uart_printstr("\r\n");
}

int main()
{
	adc_init();
	uart_init();
	while (1)
	{
		read_and_print_all_potent();
		_delay_ms(20);
	}
}
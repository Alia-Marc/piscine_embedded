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
	// No ADLAR, so we are working with 10bit register by reading ADCH then bitshift and then reading ADCL
	ADMUX = (1 << REFS0);
}

uint16_t	read_ADC_data_register(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return (ADC);
}

void	uart_printnbr(uint16_t n)
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

uint16_t	read_RV1(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Set no MUX to choose reading from ADC0
	ADMUX = (1 << REFS0);
	return (read_ADC_data_register());
}

uint16_t	read_LDR(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Set no MUX0 to choose reading from ADC1, 24-4
	ADMUX = (1 << REFS0) | (1 << MUX0);
	return (read_ADC_data_register());
}

uint16_t	read_NTC(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Set MUX1 to choose reading from ADC2, 24-4
	ADMUX = (1 << REFS0) | (1 << MUX1);
	
	return (read_ADC_data_register());
}

void	read_and_print_all_potent(void)
{
	uint16_t	RV1_byte = read_RV1();
	uint16_t	LDR_byte = read_LDR();
	uint16_t	NTC_byte = read_NTC();

	uart_printnbr(RV1_byte);
	uart_printstr(", ");
	uart_printnbr(LDR_byte);
	uart_printstr(", ");
	uart_printnbr(NTC_byte);
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
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
	ADMUX = (1 << MUX3);

	// 24-3 Set REFS0, setup ADC with 1.1v in reference for the temperature sensor
	// No ADLAR, so we are working with 10bit register by reading ADCH then bitshift and then reading ADCL
	ADMUX |= (1 << REFS0) | (1 << REFS1);
}

int16_t	read_ADC_data_register(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return (ADC);
}

void	uart_printnbr(int16_t n)
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

void	read_and_print_all_potent(void)
{
	// -327 mv, 23-8, it says it's The voltage sensitivity is approximately 1 mV/°C and the accuracy of the temperature measurement is +/- 10°C
	// Since it's 0x0160|352 when it's 25°C, 352-25=320 to get the approximative mV value for 0°C
	uart_printnbr(read_ADC_data_register() - 327);
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
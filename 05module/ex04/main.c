#include <avr/io.h>
#include <util/delay.h>


void wheel(uint8_t pos);
void init_rgb();

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

uint8_t	read_ADC_data_register(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return (ADCH);
}

uint8_t	read_RV1(void)
{
	// 24-3 Set REFS0,setup ADC with AVcc in reference
	// Set no MUX to choose reading from ADC0
	// Set ADLAR to read only 8 bit, to sync with the wheel rgb
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	return (read_ADC_data_register());
}

void	leds_to_light_up()
{
	uint8_t	RV1_byte = read_RV1();

	wheel(RV1_byte);
	if (RV1_byte < 255 / 4)
		PORTB &= ~(1 << PORTB0 | 1 << PORTB1 | 1 << PORTB2 | 1 << PORTB4);
	if (RV1_byte >= 255 / 4)
		PORTB = (1 << PORTB0);
	if (RV1_byte >= 255 / 2)
		PORTB |= (1 << PORTB1);
	if (RV1_byte >= 255 - 255 / 4)
		PORTB |= (1 << PORTB2);
	if (RV1_byte == 255)
		PORTB |= (1 << PORTB4);
}

int main()
{
	adc_init();
	init_rgb();
	while (1)
	{
		leds_to_light_up();
		_delay_ms(20);
	}
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main()
{
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
	while (1)
	{
		PORTD = (1 << PORTD5);
		_delay_ms(1000);
		PORTD = (1 << PORTD6);
		_delay_ms(1000);
		PORTD = (1 << PORTD3);
		_delay_ms(1000);
	}
}
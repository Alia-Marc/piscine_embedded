#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED (1 << PORTD5)
#define GREEN (1 << PORTD6)
#define BLUE (1 << PORTD3)

int main()
{
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
	while (1)
	{
		PORTD = RED;
		_delay_ms(1000);
		PORTD = GREEN;
		_delay_ms(1000);
		PORTD = BLUE;
		_delay_ms(1000);
		PORTD = RED | GREEN;
		_delay_ms(1000);
		PORTD = GREEN | BLUE;
		_delay_ms(1000);
		PORTD = RED | BLUE;
		_delay_ms(1000);
		PORTD = RED | GREEN | BLUE;
		_delay_ms(1000);
	}
}
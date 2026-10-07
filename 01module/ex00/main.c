#include <avr/io.h>


//avr-objdump -dS xxx.bin

void	wait_ms(uint32_t ms)
{
	uint32_t count = 0;
	while(count++ <= 16000000/1000*ms){}
}


int main() 
{
	DDRB = 1 << DDB1;

	while (1)
	{
		PORTB ^= (1 << PORTB1);
		wait_ms(500);
	}
}

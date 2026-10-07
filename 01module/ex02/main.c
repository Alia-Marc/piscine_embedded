#include <avr/io.h>

#define TOP (F_CPU / 1024)

int main() 
{

	//OC1A connected to LED D2
	DDRB |= 1 << DDB1;

	// Activate clk with prescaling 1024
	// Activate mode 14 fast PWM mode with ICR1 on TOP value
	TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12) |  (1 << WGM13);
	TCCR1A |= (1 << WGM11) | (1 << COM1A1);
	// An interrupt can be generated at each time the counter value
	// reaches the TOP value by either using the OFC1A or ICF1 flag
	// 16.1

	// In fast PWM mode 14, the counter is cleared to 0,
	// when the counter value TCNT1 matches  ICR1
	// The ICR1 define the TOP value for the counter

	ICR1 = TOP;
	OCR1A = TOP / 10;

	while (1)
	{
		
	} 
}

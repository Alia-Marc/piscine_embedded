#include <avr/io.h>

#define TOP (F_CPU / 2 / 256 - 1)

int main() 
{

	//OC1A connected to LED D2
	DDRB |= 1 << DDB1;

	// Clear timer on compare = CTC mode, 16.9.2

	// Activate lk with prescaling 256
	// Activate CTC mode
	TCCR1B |= (1 << CS12) | (1 << WGM12);


	// In CTC mode, the counter is cleared to 0,
	// when the counter value TCNT1 matches either 
	// the OCR1A or the ICR1A
	// The OCR1A define the TOP value for the counter
	OCR1A = TOP;

	// An interrupt can be generated at each time the counter value
	// reaches the TOP value by either using the OFC1A or ICF1 flag
	// 16.1
	TCCR1A |= (1 << COM1A0);
	while (1)
	{
		
	} 
}

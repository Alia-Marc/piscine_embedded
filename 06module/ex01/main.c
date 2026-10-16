#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#include "uart.h"

#define AHT20_ADDRESS 						0b0111000
#define AHT20_R (AHT20_ADDRESS << 1 | 1) /* read mode */ // Read from slave
#define AHT20_W (AHT20_ADDRESS << 1 | 0) /* write mode */// Write to slave
#define AHT20_INIT_BYTE						0b10111110
#define AHT20_INIT_PARAM_BYTE				0b00001000
#define AHT20_TRIGGER_MEASURE_BYTE			0b10101100
#define AHT20_TRIGGER_MEASURE_PARAM_BYTE	0b10101100


void print_hex_value(char c)
{
	uint8_t	n = c;
	int	n1 = n >> 4;
	int	n2 = n & 0x0F;
	if (n1 > 9)
		n1 += 7;
	uart_tx(n1 + '0');
	if (n2 > 9)
		n2 += 7;
	uart_tx(n2 + '0');
}

void i2c_init(void)
{
	/* SCL_frequency = CPU_clock / (16 + 2*(TWBR) * (PRESCALER))
	   SCL_frequency * (16 + 2*(TWBR) * (PRESCALER)) = CPU_CLOCK
	   16 + 2*(TWBR) * (PRESCALER) = CPU_CLOCK / SCL_Frequency
	   2*(TWBR) * (PRESCALER) = CPU_CLOCK / SCL_Frequency - 16
	   (TWBR) * (PRESCALER) = (CPU_CLOCK / SCL_Frequency - 16) / 2 */
	TWBR = 72;	/* Set division factor (p239) */
	TWCR |= (1 << TWEA); /* Enable Acknoledge generation */
	TWCR |= (1 << TWEN);	/* enable TWI */
	SREG = (1 << SREG_I); /* Enable global interruptions */
}

void i2c_start(void)
{
	// Send a start intruction for the master to select the bus
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)))
		;
}

void i2c_stop(void)
{
	// Send a stop intruction for the master to free the bus
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (!(TWCR & (1 << TWSTO)))
		;
}

// MR mode
int	i2c_init_read(void)
{
	i2c_start();
	// Tells the master wants to communicate in receiving mode, to the slave address in question
	TWDR = AHT20_R;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	if (TW_STATUS != TW_MR_SLA_ACK)
		return (1);
	return (0);
}

void	i2c_read(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)))
		;
	print_hex_value(TWDR);
}

uint8_t	i2c_read_last_nack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return (TWDR);
}

// MT mode
int	i2c_init_write(void)
{
	i2c_start();
	// Tells the master wants to communicate in transmiting mode, to the slave address in question
	TWDR = AHT20_W;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	if (TW_STATUS != TW_MT_SLA_ACK)
		return (1);
	return (0);
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)))
		;
}

int	init_AHT20(void)
{
	if (i2c_init_read())
		return (1);

	// 5.4.1 We check if the calibration bit is set or not. If the bit is 0, it means we need to calibrate the sensor by sending 0xbe command and the params 0x08 and 0x00
	if (!(i2c_read_last_nack() >> 3 & 1))
	{
		if (i2c_init_write())
			return (1);

		// Initialize AHT20, send init command, then the two byte parameters 5.4.1, then wait 10ms
		i2c_write(AHT20_INIT_BYTE);
		if (TW_STATUS != TW_MT_DATA_ACK)
			return (1);
		i2c_write(AHT20_INIT_PARAM_BYTE);
		if (TW_STATUS != TW_MT_DATA_ACK)
			return (1);
		i2c_write(0);
		if (TW_STATUS != TW_MT_DATA_ACK)
			return (1);
	}

	i2c_stop();
	_delay_ms(10);
	return (0);
}

int	get_AHT20_measurement(void)
{
	uint8_t	status;
	do {
		_delay_ms(10);
		if (i2c_init_read())
			return (1);
		status = i2c_read_last_nack();
	} while (status >> 7 & 1);
	if (i2c_init_read())
		return (1);
	for (int i = 0; i < 7; i++)
	{
		if (i == 6)
			print_hex_value(i2c_read_last_nack());
		else
			i2c_read();
		uart_tx(' ');
	}
	uart_printstr("\n\r");
	return (0);
}

int	trigger_AHT20_measurement(void)
{
	if (i2c_init_write())
		return(1);
	
	// Trigger measurement 5.4.2
	i2c_write(AHT20_TRIGGER_MEASURE_BYTE);
	if (TW_STATUS != TW_MT_DATA_ACK)
		return (1);
	i2c_write(AHT20_TRIGGER_MEASURE_PARAM_BYTE);
	if (TW_STATUS != TW_MT_DATA_ACK)
		return (1);
	i2c_write(0);
	if (TW_STATUS != TW_MT_DATA_ACK)
		return (1);
	_delay_ms(80); // Wait for 80ms to wait for the measurement to be completed 5.4.3
	if (get_AHT20_measurement())
		return (i2c_stop(), 1);
}

int main()
{
	_delay_ms(40);
	uart_init();
	i2c_init();
	if (init_AHT20())
		return (uart_printstr("ERROR while AHT20 initialization"), 1);
	while (1)
	{
		if (trigger_AHT20_measurement())
			return (uart_printstr("ERROR while AHT20 measurement"), 1);

		// 2.4 When the measurement frequency is too
		// high, the temperature of the sensor itself will rise,
		// which will affect the measurement accuracy. In order
		// to make its temperature rise below 0.1, the
		// activation time of AHT20 should not exceed 10% of
		// the measurement time - it is recommended to
		// measure data every 2 seconds.
		_delay_ms(2000);
	}
}
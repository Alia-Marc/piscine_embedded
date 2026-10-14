#include <avr/io.h>
#include <util/twi.h>

#include "uart.h"

#define AHT20_ADDRESS 0b0111000
#define AHT20_R (AHT20_ADDRESS << 1 | 1) /* read mode */ // Read from slave
#define AHT20_W (AHT20_ADDRESS << 1 | 0) /* write mode */// Write to slave

void	uart_print_TW_STATUS(void)
{
	switch (TW_STATUS)
	{
		case TW_START:
			uart_printstr("TW_START");
			break;
		case TW_REP_START:
			uart_printstr("TW_REP_START");
			break;
		case TW_MT_SLA_ACK:
			uart_printstr("TW_MT_SLA_ACK");
			break;
		case TW_MT_SLA_NACK:
			uart_printstr("TW_MT_SLA_NACK");
			break;
		case TW_MT_DATA_ACK:
			uart_printstr("TW_MT_DATA_ACK");
			break;
		case TW_MT_DATA_NACK:
			uart_printstr("TW_MT_DATA_NACK");
			break;
		case TW_MR_SLA_ACK:
			uart_printstr("TW_MR_SLA_ACK");
			break;
		case TW_MR_SLA_NACK:
			uart_printstr("TW_MR_SLA_NACK");
			break;
		case TW_MR_DATA_ACK:
			uart_printstr("TW_MR_DATA_ACK");
			break;
		case TW_MR_DATA_NACK:
			uart_printstr("TW_MR_DATA_NACK");
			break;
		case TW_ST_SLA_ACK:
			uart_printstr("TW_ST_SLA_ACK");
			break;
		case TW_ST_ARB_LOST_SLA_ACK:
			uart_printstr("TW_ST_ARB_LOST_SLA_ACK");
			break;
		case TW_ST_DATA_ACK:
			uart_printstr("TW_ST_DATA_ACK");
			break;
		case TW_SR_SLA_ACK:
			uart_printstr("TW_SR_SLA_ACK");
			break;
		case TW_SR_ARB_LOST_SLA_ACK:
			uart_printstr("TW_SR_ARB_LOST_SLA_ACK");
			break;
		case TW_SR_GCALL_ACK:
			uart_printstr("TW_SR_GCALL_ACK");
			break;
		case TW_SR_ARB_LOST_GCALL_ACK:
			uart_printstr("TW_SR_ARB_LOST_GCALL_ACK");
			break;
		case TW_SR_DATA_ACK:
			uart_printstr("TW_SR_DATA_ACK");
			break;
		case TW_SR_GCALL_DATA_ACK:
			uart_printstr("TW_SR_GCALL_DATA_ACK");
			break;
		case TW_MT_ARB_LOST:
			uart_printstr("TW_MT_ARB_LOST");
			break;
		case TW_SR_DATA_NACK:
			uart_printstr("TW_SR_DATA_NACK");
			break;
		case TW_SR_GCALL_DATA_NACK:
			uart_printstr("TW_SR_GCALL_DATA_NACK");
			break;
		case TW_SR_STOP:
			uart_printstr("TW_SR_STOP");
			break;
		case TW_ST_DATA_NACK:
			uart_printstr("TW_ST_DATA_NACK");
			break;
		case TW_ST_LAST_DATA:
			uart_printstr("TW_ST_LAST_DATA");
			break;
		case TW_NO_INFO:
			uart_printstr("TW_NO_INFO");
			break;
		case TW_BUS_ERROR:
			uart_printstr("TW_BUS_ERROR");
			break;
		default:
			uart_printstr("Should not happend");
			break;
	}
	uart_printstr("\n\r");
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
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	uart_print_TW_STATUS();

	// Tells the master wants to communicate in transmiting mode, to the slave address in question
	TWDR = AHT20_R;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	uart_print_TW_STATUS();
}

void i2c_stop(void)
{
	// Send a stop intruction for the master to free the bus
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (!(TWCR & (1 << TWSTO)))
		;
	uart_print_TW_STATUS();
}

int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	while (1)
	{


	}
}
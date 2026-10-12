#include <util/twi.h>

#include "i2c.h"

static int i2c_start(void);

void i2c_init(void)
{
	/* SCL_frequency = CPU_clock / (16 + 2*(TWBR) * (PRESCALER))
	   SCL_frequency * (16 + 2*(TWBR) * (PRESCALER)) = CPU_CLOCK
	   16 + 2*(TWBR) * (PRESCALER) = CPU_CLOCK / SCL_Frequency
	   2*(TWBR) * (PRESCALER) = CPU_CLOCK / SCL_Frequency - 16
	   (TWBR) * (PRESCALER) = (CPU_CLOCK / SCL_Frequency - 16) / 2 */
	TWBR = 72;	/* Set division factor (p239) */
	TWAR = I2C_SLAVE_ADDRESS << 1;
	TWCR |= (1 << TWEA); /* Enable Acknoledge generation */
	TWCR |= (1 << TWEN);	/* enable TWI */
	SREG = (1 << SREG_I); /* Enable global interruptions */
}

int i2c_enable_mt_mode(void)
{
	if (!i2c_start())
		return 0;


	TWAR = I2C_MASTER_ADDRESS << 1;
	TWDR = I2C_SLAVE_ADDRESS << 1 | 0 /* write mode */;
	TWCR = (1 << TWINT) | (1 << TWEN);
	for (uint32_t i = 0; !(TWCR & (1 << TWINT)) && i < F_CPU / 6 / 10; i++)
		;
	return (TW_STATUS == TW_MT_SLA_ACK);
}

int i2c_enable_mr_mode(void)
{
	if (!i2c_start())
		return 0;

	TWAR = I2C_MASTER_ADDRESS << 1;
	TWDR = I2C_SLAVE_ADDRESS << 1 | 1 /* read mode */;
	TWCR = (1 << TWINT) | (1 << TWEN);
	for (uint32_t i = 0; !(TWCR & (1 << TWINT)) && i < F_CPU / 6 / 10; i++)
		;
	return (TW_STATUS == TW_MR_SLA_ACK);
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (!(TWCR & (1 << TWSTO)))
		;
}

int i2c_send_byte(uint8_t byte)
{
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	for (uint32_t i = 0; !(TWCR & (1 << TWINT)) && i < F_CPU / 6 / 10; i++)
		;
	return (TW_STATUS == TW_MT_DATA_ACK || TW_STATUS == TW_MT_DATA_NACK || TW_STATUS == TW_ST_DATA_ACK || TW_STATUS == TW_ST_DATA_NACK);
}

int i2c_action_to_do(void)
{
	return TWCR & _BV(TWINT);
}

int i2c_data_received(void)
{
	switch (TW_STATUS)
	{
		case TW_MR_DATA_ACK:
		case TW_SR_DATA_ACK:
			return 1;
		default:
			return 0;
	}
}

int i2c_addr_received(void)
{
	switch (TW_STATUS)
	{
		case TW_ST_SLA_ACK:
		case TW_ST_ARB_LOST_SLA_ACK:
		case TW_SR_SLA_ACK:
		case TW_SR_ARB_LOST_SLA_ACK:
		case TW_SR_GCALL_ACK:
		case TW_SR_ARB_LOST_GCALL_ACK:
			return 1;
		default:
			return 0;
	}
}

void i2c_next_packet(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
}

void i2c_nack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN);
}

uint8_t i2c_get_data(void)
{
	return TWDR;
}

t_i2c_role i2c_get_role(void)
{
	switch (TW_STATUS)
	{
		case TW_START:
		case TW_REP_START:
			return MASTER_NONE;
		case TW_MT_SLA_ACK:
		case TW_MT_SLA_NACK:
		case TW_MT_DATA_ACK:
		case TW_MT_DATA_NACK:
			return MASTER_TRANSMITTER;
		case TW_MR_SLA_ACK:
		case TW_MR_SLA_NACK:
		case TW_MR_DATA_ACK:
		case TW_MR_DATA_NACK:
			return MASTER_RECEIVER;
		case TW_ST_SLA_ACK:
		case TW_ST_ARB_LOST_SLA_ACK:
		case TW_ST_DATA_ACK:
			return SLAVE_TRANSMITTER;
		case TW_SR_SLA_ACK:
		case TW_SR_ARB_LOST_SLA_ACK:
		case TW_SR_GCALL_ACK:
		case TW_SR_ARB_LOST_GCALL_ACK:
		case TW_SR_DATA_ACK:
		case TW_SR_GCALL_DATA_ACK:
			return SLAVE_RECEIVER;
		case TW_MT_ARB_LOST:
		case TW_SR_DATA_NACK:
		case TW_SR_GCALL_DATA_NACK:
		case TW_SR_STOP:
		case TW_ST_DATA_NACK:
		case TW_ST_LAST_DATA:
		case TW_NO_INFO:
		case TW_BUS_ERROR:
			return SLAVE_NONE;
		default:
			return UNKNOWN;
	}
}

static int i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return (TW_STATUS == TW_START) || (TW_STATUS == TW_REP_START);
}

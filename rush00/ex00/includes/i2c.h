#ifndef I2C_H
# define I2C_H

# define I2C_MASTER_ADDRESS 42
# define I2C_SLAVE_ADDRESS 121

typedef enum e_i2c_role
{
	UNKNOWN,
	MASTER_NONE,
	MASTER_TRANSMITTER,
	MASTER_RECEIVER,
	SLAVE_NONE,
	SLAVE_TRANSMITTER,
	SLAVE_RECEIVER,
} t_i2c_role;

void i2c_init(void);
int i2c_enable_mt_mode(void);
int i2c_enable_mr_mode(void);
void i2c_stop(void);
int i2c_send_byte(uint8_t byte);
int i2c_action_to_do(void);
int i2c_data_received(void);
int i2c_addr_received(void);
void i2c_next_packet(void);
void i2c_nack(void);
t_i2c_role i2c_get_role();
uint8_t i2c_get_data(void);

#endif

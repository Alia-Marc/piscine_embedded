#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include "i2c.h"

#define IO_MODE_OUTPUT(_PORT_ZONE, _PORT_N) (DDR##_PORT_ZONE |= (1 << DD##_PORT_ZONE##_PORT_N))
#define IO_MODE_INPUT(_PORT_ZONE, _PORT_N) (DDR##_PORT_ZONE &= ~(1 << DD##_PORT_ZONE##_PORT_N))

#define SET_PORT_TO_HIGH(_PORT_ZONE, _PORT_N) (PORT##_PORT_ZONE |= (1 << PB##_PORT_N))
#define SET_PORT_TO_LOW(_PORT_ZONE, _PORT_N) (PORT##_PORT_ZONE &= ~(1 << PB##_PORT_N))
#define SET_PORT_TO_INVERT(_PORT_ZONE, _PORT_N) (PORT##_PORT_ZONE ^= (1 << PB##_PORT_N))

#define IS_PORT_HIGH(_PORT_ZONE, _PORT_N) (PIN##_PORT_ZONE & (1 << PB##_PORT_N))
#define IS_PORT_LOW(_PORT_ZONE, _PORT_N) (!(PIN##_PORT_ZONE & (1 << PB##_PORT_N)))

#define PACKET_BECOME_MASTER	0b00000100
#define PACKET_BUTTON_PUSHED 	0b00000001
#define PACKET_BUTTON_RELEASED	0b00000000
#define PACKET_YOU_LOSE			0b00000010
#define PACKET_YOU_WIN			0b00000011


typedef enum e_phase
{
	NO_MASTER,
	MASTER_WAIT_FOR_GAME_START,
	SLAVE_WAIT_FOR_GAME_START,
	MASTER_WAIT_FOR_BUTTON_PUSH,
	SLAVE_WAIT_FOR_BUTTON_PUSH,
} t_phase;

typedef struct s_state
{
	t_phase phase;
	union
	{
		struct
		{
			uint8_t is_slave_button_pushed;
		} master_wait_for_game_start_data;
		struct
		{
			uint8_t was_button_pushed;
		} slave_wait_for_game_start_data;
		struct
		{
			uint8_t wait_time;
			uint8_t is_slave_button_pushed;
		} master_wait_for_button_push_data;
		struct
		{
			uint8_t wait_time;
		} slave_wait_for_button_push_data;
	};
} t_state;


static void setup(void);
static void loop(void);
static void handle_state(t_state *state);
static void handle_no_master(t_state *state);
static void handle_master_wait_for_game_start(t_state *state);
static void handle_slave_wait_for_game_start(t_state *state);
static void handle_master_wait_for_button_push(t_state *state);
static void handle_slave_wait_for_button_push(t_state *state);
static void game(uint8_t wait_time);
static int send_button_state_packet(int is_button_pushed);
static int is_debounced_button_pushed(void);
static void reset_lights(void);

int time = 0;
int lights_out = 0; //TODO Do not forget to reset at the end


static void setup(void)
{
	IO_MODE_OUTPUT(B, 0);
	IO_MODE_OUTPUT(B, 1);
	IO_MODE_OUTPUT(B, 2);
	IO_MODE_OUTPUT(B, 4);
	IO_MODE_OUTPUT(D, 3);
	IO_MODE_OUTPUT(D, 5);
	IO_MODE_OUTPUT(D, 6);
	i2c_init();
	SET_PORT_TO_HIGH(D, 3);
}

static void loop(void)
{
	t_state state = {0};

	while (1)
	{
		handle_state(&state);
		_delay_ms(10);
		if (!lights_out)
			time += 30;
	}
}

static void handle_state(t_state *state)
{
	switch(state->phase)
	{
		case NO_MASTER:
			handle_no_master(state);
			break;
		case MASTER_WAIT_FOR_GAME_START:
			handle_master_wait_for_game_start(state);
			break;
		case SLAVE_WAIT_FOR_GAME_START:
			handle_slave_wait_for_game_start(state);
			break;
		case MASTER_WAIT_FOR_BUTTON_PUSH:
			handle_master_wait_for_button_push(state);
			break;
		case SLAVE_WAIT_FOR_BUTTON_PUSH:
			handle_slave_wait_for_button_push(state);
			break;
		default:
			/* should never occur */
			break;
	}
	_delay_ms(20);
}

static void soft_reset(void)
{
	WDTCSR |= (1 << WDE);
	while (1) ;
}

static void handle_no_master(t_state *state)
{
	if (i2c_action_to_do())
	{
		if (TW_STATUS == TW_BUS_ERROR || TW_STATUS == TW_NO_INFO)
			soft_reset();
		if (i2c_data_received())
		{
			if (TWDR == 255)
				soft_reset();
			if (i2c_get_data() == PACKET_BECOME_MASTER)
			{
				*state = (t_state) {.phase = SLAVE_WAIT_FOR_GAME_START};
				SET_PORT_TO_HIGH(D, 3);
			}
		}
		i2c_next_packet();
	}
	if (i2c_get_role() == SLAVE_NONE && is_debounced_button_pushed())
	{
		if (!i2c_enable_mt_mode())
		{
			*state = (t_state) {.phase = SLAVE_WAIT_FOR_GAME_START};
			return;
		}
		if (!i2c_send_byte(PACKET_BECOME_MASTER))
		{
			*state = (t_state) {.phase = SLAVE_WAIT_FOR_GAME_START};
			return;
		}
		if (!i2c_enable_mr_mode())
		{
			*state = (t_state) {.phase = SLAVE_WAIT_FOR_GAME_START};
			return;
		}
		*state = (t_state) {.phase = MASTER_WAIT_FOR_GAME_START};
	}
}

static void handle_master_wait_for_game_start(t_state *state)
{
	if (i2c_action_to_do())
	{
		if (TW_STATUS == TW_BUS_ERROR || TW_STATUS == TW_NO_INFO)
			soft_reset();

		if (i2c_data_received())
		{
			if (TWDR == 255)
				soft_reset();
			state->master_wait_for_game_start_data.is_slave_button_pushed = (TWDR == PACKET_BUTTON_PUSHED);
		}
		if (!(state->master_wait_for_game_start_data.is_slave_button_pushed && is_debounced_button_pushed()))
			i2c_next_packet(); 
	}
	/* When both players press the button, the game starts. */
	if (state->master_wait_for_game_start_data.is_slave_button_pushed && is_debounced_button_pushed())
	{
		uint8_t wait_time = 5;

		i2c_nack();
		while (!(TWCR & (1 << TWINT)))
			;
		if (!i2c_enable_mt_mode())
		{
			return;
		}
		if (!i2c_send_byte(wait_time))
		{
			return;
		}
		if (!i2c_enable_mr_mode()) /* TODO Check if it is a problem */
		{
			return;
		}
		*state = (t_state) {.phase = MASTER_WAIT_FOR_BUTTON_PUSH};
		state->master_wait_for_button_push_data.wait_time = wait_time;
		time = 0;
		SET_PORT_TO_LOW(D, 3);
	}
}

static void handle_slave_wait_for_game_start(t_state *state)
{
	uint8_t is_button_pushed = is_debounced_button_pushed();

	if (i2c_get_role() == SLAVE_NONE && i2c_action_to_do())
		i2c_next_packet();
	else if (i2c_addr_received())
		i2c_next_packet();
	if (i2c_get_role() == SLAVE_TRANSMITTER)
	{
		if (!send_button_state_packet(is_button_pushed))
			soft_reset();
		state->slave_wait_for_game_start_data.was_button_pushed = is_button_pushed;
	}
	else if (i2c_get_role() == SLAVE_RECEIVER)
	{
		if (i2c_action_to_do())
		{
			if (i2c_data_received())
			{
				*state = (t_state) {.phase = SLAVE_WAIT_FOR_BUTTON_PUSH};
				state->slave_wait_for_button_push_data.wait_time = i2c_get_data();
				time = 0;
				SET_PORT_TO_LOW(D, 3);
			}
			i2c_next_packet();
		}
	}

}

static void handle_master_wait_for_button_push(t_state *state)
{
	int winner = 0;

	game(state->master_wait_for_button_push_data.wait_time);	

	if (i2c_action_to_do())
	{
		if (TW_STATUS == TW_BUS_ERROR || TW_STATUS == TW_NO_INFO)
			soft_reset();
		if (i2c_data_received())
		{
			if (TWDR == 255)
				soft_reset();
			state->master_wait_for_button_push_data.is_slave_button_pushed = (TWDR == PACKET_BUTTON_PUSHED);
		}
		i2c_next_packet();
	}
	/* The fastest player to press the button at the end of the countdown wins the game. */
	if (is_debounced_button_pushed() && time >= 1000)
	{
		if (lights_out)
		{
			SET_PORT_TO_HIGH(D, 6);
			winner = 1;
		}
		else
		{
			SET_PORT_TO_HIGH(D, 5);
			winner = 2;
		}
	}
	else if (state->master_wait_for_button_push_data.is_slave_button_pushed && time >= 1000)
	{
		if (lights_out)
		{
			SET_PORT_TO_HIGH(D, 5);
			winner = 2;
		}
		else
		{
			SET_PORT_TO_HIGH(D, 6);
			winner = 1;
		}
	}
	if (winner)
	{
		i2c_nack();
		while (!(TWCR & (1 << TWINT)))
			;
		if (!i2c_enable_mt_mode())
			return;
		if (winner == 1)
		{
			if (!i2c_send_byte(PACKET_YOU_LOSE))
				return;
		}
		else
		{
			if (!i2c_send_byte(PACKET_YOU_WIN))
				return;
		}
		_delay_ms(3000);
		soft_reset(); /* Another game can now start. */
		i2c_enable_mr_mode();
		reset_lights();
		SET_PORT_TO_HIGH(D, 3);
		lights_out = 0;
		*state = (t_state) {.phase = MASTER_WAIT_FOR_GAME_START};
	}
}

static void handle_slave_wait_for_button_push(t_state *state)
{
	uint8_t is_button_pushed = is_debounced_button_pushed();

	if (i2c_get_role() == SLAVE_NONE && i2c_action_to_do())
	{
		i2c_next_packet();
	}
	if (i2c_addr_received())
	{
		i2c_next_packet();
	}
	if (i2c_get_role() == SLAVE_TRANSMITTER)
	{
		if (!send_button_state_packet(is_button_pushed))
			soft_reset();
	}
	else if (i2c_get_role() == SLAVE_RECEIVER)
	{
		if (i2c_action_to_do())
		{
			if (i2c_data_received())
			{
				if (i2c_get_data() == PACKET_YOU_LOSE)
					SET_PORT_TO_HIGH(D, 5);
				else
					SET_PORT_TO_HIGH(D, 6);
				i2c_next_packet();
				_delay_ms(3000);
				soft_reset(); /* Another game can now start. */
				reset_lights();
				SET_PORT_TO_HIGH(D, 3);
				lights_out = 0;
				*state = (t_state) {.phase = SLAVE_WAIT_FOR_GAME_START};
				return;
			}
			i2c_next_packet();
		}
	}
	game(state->slave_wait_for_button_push_data.wait_time);	
}

/* A countdown is displayed on the LEDs. */
static void game(uint8_t wait_time)
{
	if (time >= 5000 + wait_time * 250)
	{
		SET_PORT_TO_LOW(B, 4);
		SET_PORT_TO_LOW(B, 2);
		SET_PORT_TO_LOW(B, 1);
		SET_PORT_TO_LOW(B, 0);
		lights_out = 1;
	}
	else
	{
		if (time >= 2000)
		{
			SET_PORT_TO_HIGH(B, 4);
		}
		if (time >= 3000)
		{
			SET_PORT_TO_HIGH(B, 2);
		}
		if (time >= 4000)
		{
			SET_PORT_TO_HIGH(B, 1);
		}
		if (time >= 5000)
		{
			SET_PORT_TO_HIGH(B, 0);
		}
	}
}

static int send_button_state_packet(int is_button_pushed)
{
	if (is_button_pushed)
		return i2c_send_byte(PACKET_BUTTON_PUSHED);
	else
		return i2c_send_byte(PACKET_BUTTON_RELEASED);
}

static int is_debounced_button_pushed(void)
{
	int button_first_state, button_second_state;

	do
	{
		button_first_state = IS_PORT_LOW(D, 2);
		_delay_ms(10);
		button_second_state = IS_PORT_LOW(D, 2);
	} while (button_first_state != button_second_state);
	return button_first_state;
}

static void reset_lights(void)
{
	SET_PORT_TO_LOW(B, 0);
	SET_PORT_TO_LOW(B, 1);
	SET_PORT_TO_LOW(B, 2);
	SET_PORT_TO_LOW(B, 4);
	SET_PORT_TO_LOW(D, 3);
	SET_PORT_TO_LOW(D, 5);
	SET_PORT_TO_LOW(D, 6);
}

int main()
{
	setup();
	loop();
}


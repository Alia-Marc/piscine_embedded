NAME			=	prog.hex
BIN_NAME		=	prog.bin

SRCS			=	main.c \
					i2c.c

_OBJS			=	${SRCS:.c=.o}
OBJS			=	$(addprefix build/, $(_OBJS))

F_CPU			=	16000000UL
UART_BAUDRATE	=	115200

CC				=	avr-gcc
CFLAGS			=	-Wall -Werror -Wextra -mmcu=atmega328p -Os -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(UART_BAUDRATE) -I includes

OBJCOPY			=	avr-objcopy
OBJCOPY_FLAGS	=	-O ihex

AVRDUDE			=	avrdude
AVRDUDE_DEVICE	=	/dev/ttyUSB0
AVRDUDE_DEVICE_2=	/dev/ttyUSB1
AVRDUDE_FLAGS	=	-P $(AVRDUDE_DEVICE) -p m328p -c arduino -U flash:w:$(NAME)
AVRDUDE_FLAGS_2	=	-P $(AVRDUDE_DEVICE_2) -p m328p -c arduino -U flash:w:$(NAME)


all		:	hex flash

duo		:	hex flash flash_2

hex		:	$(NAME)

$(NAME)	:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o build/$(BIN_NAME)
	$(OBJCOPY) $(OBJCOPY_FLAGS) build/$(BIN_NAME) $(NAME)
	
build/%.o	:	srcs/%.c
	@if [ ! -d $(dir $@) ]; then\
		mkdir -p $(dir $@);\
	fi
	$(CC) ${CFLAGS} $< -c -o $@

flash	:	$(NAME)
	$(AVRDUDE) $(AVRDUDE_FLAGS)

flash_2	:	$(NAME)
	$(AVRDUDE) $(AVRDUDE_FLAGS_2)

clean	:	
	rm -Rf build/
	rm -f ${NAME}

re		:	clean all

.PHONY	:	all hex clean re

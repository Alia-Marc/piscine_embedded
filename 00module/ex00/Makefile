F_CPU = 16000000UL

#Pour le commun -> /dev/ttyUSB0 
PORT = /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0 


all: hex flash

hex:
	avr-gcc -mmcu=atmega328p -DF_CPU=$(F_CPU) main.c -o main.bin
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

flash:
	avrdude -c arduino -b 115200 -p m328p -P $(PORT) -U flash:w:main.hex

clean: 
	rm -f main.hex
	rm -f main.bin
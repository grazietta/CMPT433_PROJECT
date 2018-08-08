all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L message.c list.c main.c joystick.c pot.c clap.c -o proj -lm -pthread
	cp proj $(HOME)/cmpt433/public/
clean:
	rm proj

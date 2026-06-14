test: tests/test.c src/cpu.c src/memory.c
	gcc -Wall -Wextra -g -Iinclude \
		tests/test.c \
		src/cpu.c \
		src/memory.c \
		-o test_emu
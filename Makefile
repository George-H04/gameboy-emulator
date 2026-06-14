test_nop: tests/test_nop.c src/cpu.c src/memory.c
	gcc -Wall -Wextra -g -Iinclude \
		tests/test_nop.c \
		src/cpu.c \
		src/memory.c \
		-o test_nop
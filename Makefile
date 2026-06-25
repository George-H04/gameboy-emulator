test: tests/test.c src/cpu.c src/memory.c
	gcc -Wall -Wextra -g -Iinclude \
		tests/test.c \
		src/cpu.c \
		src/memory.c \
		-o test_emu
prod: src/main.c src/cpu.c src/memory.c src/romloader.c
	gcc -Wall -Wextra -g -Iinclude \
		src/main.c \
		src/cpu.c \
		src/romloader.c \
		src/memory.c \
		-o emu
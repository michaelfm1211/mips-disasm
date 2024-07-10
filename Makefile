CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

.PHONY: all
all: CFLAGS += -O3
all: mips-disasm

.PHONY: dev
dev: CFLAGS += -O0 -fsanitize=address -fsanitize=undefined
dev: mips-disasm

mips-disasm: mips-disasm.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf mips-disasm


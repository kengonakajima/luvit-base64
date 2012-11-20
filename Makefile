LUVIT = luvit
CFLAGS   := -O3 $(shell $(LUVIT) --cflags | sed s/-Werror//)
LIBFLAGS  := $(shell $(LUVIT) --libs )

all: base64.luvit

base64.luvit: base64.c
	gcc $(LIBFLAGS) -g $(CFLAGS) -I. -o $@ $^

test:
	$(LUVIT) test.lua 

clean:
	rm -rf *.luvit *.dSYM

.PHONY: all clean test


LUVIT = luvit
CFLAGS   := $(shell $(LUVIT) --cflags | sed s/-Werror//)
LIBFLAGS  := $(shell $(LUVIT) --libs )
OUT = base64.luvit

all: $(OUT)

$(OUT): base64.c
	gcc $(LIBFLAGS) -g $(CFLAGS) -I. -o $@ $^

test: programtest


programtest: $(OUT)
	$(LUVIT) test.lua 

install_luvit :
	cd /tmp; rm -rf luvit; git clone https://github.com/luvit/luvit.git; cd luvit; make; sudo make install

clean:
	rm -rf *.luvit *.dSYM

.PHONY: all clean test


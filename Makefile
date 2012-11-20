LUVIT = luvit
CFLAGS   := -O3 $(shell $(LUVIT) --cflags | sed s/-Werror//)
LIBFLAGS  := $(shell $(LUVIT) --libs )
OUT = base64.luvit

LUVITURL = https://raw.github.com/luvit/luvit-releases/master/0.5.0/luvit-0.5.0.tar.gz

all: $(OUT)

$(OUT): base64.c
	gcc $(LIBFLAGS) -g $(CFLAGS) -I. -o $@ $^

test: programtest


programtest: $(OUT)
	$(LUVIT) test.lua 

install_luvit :
	cd /tmp; rm -rf luvit; curl $(LUVITURL) > luvit.tgz; ls -l; tar zxf luvit.tgz; ls -l; cd luvit-0.5.0; make; sudo make install



clean:
	rm -rf *.luvit *.dSYM

.PHONY: all clean test


# Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/Makefile, 2019-07-26 Friday 16:50:46 nico>

CFLAGS += -O3 -std=c99 -Wall -Wextra
#probably should use something else to set these
LDFLAGS += -dynamiclib -framework Foundation

src := $(shell find . -name \*.[cm])

EEexLoader.dylib: $(addsuffix .o, $(basename $(src)))
	$(LINK.o) $^ $(OUTPUT_OPTION)

%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
#already know what the compiler will say so silence objc warnings
%.o: %.m 
	$(COMPILE.m) -w $(OUTPUT_OPTION) $<

.PHONY: clean
clean:
	rm -rf *.o

# Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/Makefile, 2019-09-20 Friday 00:58:26 nico>

CFLAGS += -O3 -flto -std=c99 -Wall -Wextra
LDFLAGS += -dynamiclib -framework Foundation

src := $(shell find . -name \*.[cm])

EEexLoader.dylib: $(addsuffix .o, $(basename $(src)))
	$(LINK.o) $^ $(OUTPUT_OPTION)

%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
%.o: %.m 
	$(COMPILE.m) $(OUTPUT_OPTION) $<

.PHONY: clean
clean:
	rm -rf *.o

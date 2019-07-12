# Not portable.
# This is just to save myself trouble.
# Other compilation settings untested.
# Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/Makefile, 2019-07-08 Monday 20:17:08 nico>

EExMacLoader: EEex_Logger.o EEex_Runtime.o EEex_mach.o
	clang -dynamiclib EEex_Runtime.o EEex_mach.o EEex_Logger.o -framework Foundation -framework AudioUnit -o EEexLoader.dylib

EEex_Runtime.o:
	clang -O0 -c -o EEex_Runtime.o -Wall -Wextra EEex_Runtime.c

EEex_mach.o:
	clang -O0 -c -o EEex_mach.o -Wall -Wextra EEex_mach.c

EEex_Logger.o:
	clang -O3 -c -o EEex_Logger.o -w EEex_Logger.m

.PHONY: clean
clean:
	rm -f EEexLoader.dylib EEex_Runtime.o EEex_Logger.o EEex_mach.o

# Not portable.
# This is just to save myself trouble.
# Other compilation settings untested.
# Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/Makefile, 2019-07-15 Monday 01:46:06 nico>

EExMacLoader: EEex_Logger.o EEex_Init.o EEex_Mach.o EEex_Lua.o
	clang -dynamiclib EEex_Init.o EEex_Mach.o EEex_Lua.o EEex_Logger.o -framework Foundation -o EEexLoader.dylib

EEex_Init.o:
	clang -O3 -c -o EEex_Init.o -std=c99 -Wall -Wextra EEex_Init.c

EEex_Mach.o:
	clang -O3 -c -o EEex_Mach.o -std=c99 -Wall -Wextra EEex_Mach.c

EEex_Lua.o:
	clang -O3 -c -o EEex_Lua.o -std=c99 -Wall -Wextra EEex_Lua.c

EEex_Logger.o:
	clang -O3 -c -o EEex_Logger.o -w EEex_Logger.m

.PHONY: clean
clean:
	rm -f EEexLoader.dylib EEex_Init.o EEex_Logger.o EEex_Mach.o EEex_Lua.o

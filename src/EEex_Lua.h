#ifndef __EEEX_LUA_H_
#define __EEEX_LUA_H_

int EEex_lua_init(void*);
int EEex_lua_dump_stack(void*);

int EEex_lua_read_dword(void*);
int EEex_lua_write_byte(void*);

#endif /* !__EEEX_LUA_H_ */

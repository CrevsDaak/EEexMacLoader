#ifndef __EEEX_MACH_H__
#define __EEEX_MACH_H__

int EEex_protect(void*, vm_prot_t, bool);
extern inline int EEex_write_byte(void*, int8_t);
extern inline int EEex_write_word(void*, int16_t);
extern inline int EEex_write_dword(void*, int32_t);
extern inline int EEex_write_qword(void*, int64_t);

#endif /* !__EEEX_MACH_H__ */

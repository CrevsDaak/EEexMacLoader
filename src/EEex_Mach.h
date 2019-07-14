#ifndef __EEEX_MACH_H__
#define __EEEX_MACH_H__

int EEex_protect(void*, vm_prot_t, bool);
int EEex_write(void*, void*, uint32_t);
int EEex_read(void*, void*, uint32_t);

#endif /* !__EEEX_MACH_H__ */

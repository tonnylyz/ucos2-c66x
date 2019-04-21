#ifndef UCOS2_C66X_FAT32_H
#define UCOS2_C66X_FAT32_H

#include <types.h>

void fat32_init(void);

u32 fat32_file_size(char *filename);

u32 fat32_read_to_mem(char *filename, u8 *dest, u32 size);

#endif //UCOS2_C66X_FAT32_H

#ifndef UCOS2_C66X_MBR_H
#define UCOS2_C66X_MBR_H

#include <types.h>

void mbr_init(void *sector);

u32 mbr_find_fat32_part();

#endif //UCOS2_C66X_MBR_H

#ifndef UCOS2_ARM_MMC_H
#define UCOS2_ARM_MMC_H

#include <types.h>

int mmc_read_sector(u32 lba, u8 *buf);

#endif //UCOS2_ARM_MMC_H

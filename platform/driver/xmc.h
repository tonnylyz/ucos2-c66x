#ifndef UCOS2_C66X_XMC_H
#define UCOS2_C66X_XMC_H

#define XMC_INDEX_NUM 16

#include <partition_conf.h>
#include <types.h>

#define XMC_MPAXL0 (0x08000000)
#define XMC_MPAXH0 (0x08000004)
#define XMC_MPAXL1 (0x08000008)
#define XMC_MPAXH1 (0x0800000c)
#define XMC_XMPFAR (0x08000200)
#define XMC_XMPFSR (0x08000204)
#define XMC_XMPFCR (0x08000208)

void xmc_init(void);

u8 xmc_segment_allocate(u32 addr, u32 size);

void xmc_segment_activate(u8 index);

void xmc_invalidate_buffer(void);

#endif //UCOS2_C66X_XMC_H

#ifndef UCOS2_C66X_XMC_H
#define UCOS2_C66X_XMC_H

#define XMC_SEGMENT_NUM 16

#include <partition_conf.h>
#include <types.h>

void xmc_init();

void xmc_mem_map_dump();

u8 xmc_segment_allocate(memory_conf_t *layout);

void xmc_segment_activate(u8 index);

#endif //UCOS2_C66X_XMC_H

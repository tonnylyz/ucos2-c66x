//
// Created by Tonny on 12/26/2018.
//

#include <printf.h>
#include <partition_conf.h>
#include <os_cpu.h>
#include "xmc.h"

#define XMC_MPAXL0 (0x08000000)
#define XMC_MPAXH0 (0x08000004)
#define XMC_MPAXL1 (0x08000008)
#define XMC_MPAXH1 (0x0800000c)


#define XMC_SEGMENT_SIZE_512K   0b10010
#define XMC_SEGMENT_SIZE_1M     0b10011
#define XMC_SEGMENT_SIZE_2G     0b11110

#define XMC_SEGMENT_PERM_KERN 0b111000
#define XMC_SEGMENT_PERM_USER   0b111111


#define XMC_INDEX_RESERVED  1
#define XMC_INDEX_FREE      0
#define XMC_INDEX_PART      2

static int xmc_indices[XMC_SEGMENT_NUM];

void xmc_mem_map_dump() {
    int i;
    u32 addr;
    for (i = 0; i < XMC_SEGMENT_NUM; i++) {
        addr = XMC_MPAXL0 + i * 8;
        printf("MPAX segment %d XMPAXL%d [%08x]->[%08x]\n", i, i, addr, *((u32 volatile *) addr));
        printf("MPAX segment %d XMPAXH%d [%08x]->[%08x]\n", i, i, addr + 4, *((u32 volatile *) (addr + 4)));
    }
}

static void xmc_segment_map(int index, u32 start_addr, u32 segment_size, u32 perm) {
    u32 volatile *l;
    u32 volatile *h;
    l = (u32 volatile *) (XMC_MPAXL0 + index * 8);
    h = (u32 volatile *) (XMC_MPAXH0 + index * 8);

    /* Note: XMC do support map to different address,
     * but here map to the same address */
    *l = ((start_addr >> 4) & ~0xff) | perm;
    *h = (start_addr & ~0xfff) | segment_size;

    //printf("%d: *l=[%08x] *h=[%08x]\n", id, *l, *h);
}

void xmc_init() {
    u32 i;
    /* these settings are based on linker.cmd */

    // default map: kernel only
    xmc_segment_map(0, 0x00000000, XMC_SEGMENT_SIZE_2G, XMC_SEGMENT_PERM_USER);
    xmc_segment_map(1, 0x80000000, XMC_SEGMENT_SIZE_2G, XMC_SEGMENT_PERM_USER);

    // partition share map
    xmc_segment_map(2, 0x95200000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_USER);
    // TODO: make share segment read only


    xmc_indices[0] = XMC_INDEX_RESERVED;
    xmc_indices[1] = XMC_INDEX_RESERVED;
    
    xmc_indices[2] = XMC_INDEX_RESERVED;

    for (i = 3; i < XMC_SEGMENT_NUM; i++) {
        xmc_indices[i] = XMC_INDEX_FREE;
    }
}


static inline u32 xmc_size2segment_size(u32 size) {
    if (size == 0x100000) {
        return XMC_SEGMENT_SIZE_1M;
    } else {
        printf("xmc_size2segment_size: size %x\n", size);
        panic("Not supported segment size");
    }
}

static inline void xmc_segment_enable(u8 index) {
    u32 volatile *l;
    l = (u32 volatile *) (XMC_MPAXL0 + index * 8);
    *l = (*l & (~0b111111)) | XMC_SEGMENT_PERM_USER;
}

static inline void xmc_segment_disable(u8 index) {
    u32 volatile *l;
    l = (u32 volatile *) (XMC_MPAXL0 + index * 8);
    *l = (*l & (~0b111111)) | XMC_SEGMENT_PERM_KERN;
}

static u8 xmc_segment_find_free() {
    u8 i;
    for (i = 0; i < XMC_SEGMENT_NUM; i++) {
        if (xmc_indices[i] == XMC_INDEX_FREE) {
            return i;
        }
    }
    return i;
}

u8 xmc_segment_allocate(memory_conf_t *layout) {
    u8 index;
    index = xmc_segment_find_free();
    if (index == XMC_SEGMENT_NUM) {
        panic("Cannot Allocate XMC segment");
    }
    xmc_indices[index] = XMC_INDEX_PART;
    xmc_segment_map(index, layout->address, xmc_size2segment_size(layout->size), XMC_SEGMENT_PERM_KERN);
    return index;
}


void xmc_segment_activate(u8 index) {
    u8 i;
    if (index >= XMC_SEGMENT_NUM || xmc_indices[index] != XMC_INDEX_PART) {
        panic("xmc_segment_activate: Illegal xmc index\n");
    }
    for (i = 0; i < XMC_SEGMENT_NUM; i++) {
        if (xmc_indices[i] == XMC_INDEX_PART) {
            xmc_segment_disable(i);
        }
    }
    xmc_segment_enable(index);
}

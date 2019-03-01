//
// Created by Tonny on 12/26/2018.
//

#include <printf.h>
#include <partition_conf.h>
#include <os_cpu.h>
#include "xmc.h"
#include "mmio.h"


#define XMC_XPFCMD (0x08000300)


#define XMC_SEGMENT_SIZE_256K   0b10001
#define XMC_SEGMENT_SIZE_512K   0b10010
#define XMC_SEGMENT_SIZE_1M     0b10011
#define XMC_SEGMENT_SIZE_16M    0b10111
#define XMC_SEGMENT_SIZE_2G     0b11110
#define XMC_SEGMENT_SIZE_4G     0b11111

/*                                               K  U
 *                                              RWXRWX*/
#define XMC_SEGMENT_PERM_KERN_RWX_USER____    0b111000
#define XMC_SEGMENT_PERM_KERN_RWX_USER_RWX    0b111111
#define XMC_SEGMENT_PERM_KERN_RWX_USER_R_X    0b111101
#define XMC_SEGMENT_PERM_KERN_RWX_USER_R__    0b111100
#define XMC_SEGMENT_PERM_KERN_RWX_USER_RW_    0b111110

#define XMC_INDEX_RESERVED  1
#define XMC_INDEX_FREE      0
#define XMC_INDEX_PART      2

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
static int xmc_indices[XMC_SEGMENT_NUM];

static u32 xmc_values_l[XMC_SEGMENT_NUM];
static u32 xmc_values_h[XMC_SEGMENT_NUM];

void xmc_invalidate_buffer() {
    *(u32 volatile *)XMC_XPFCMD = 0b1001;
    barrier();
}

void xmc_mem_map_dump() {
    int i;
    u32 addr;
    for (i = 0; i < XMC_SEGMENT_NUM; i++) {
        addr = XMC_MPAXL0 + i * 8;
        printf("MPAX segment %d XMPAXL%d [%08x]->[%08x]\n", i, i, addr, *((u32 volatile *) addr));
        printf("MPAX segment %d XMPAXH%d [%08x]->[%08x]\n", i, i, addr + 4, *((u32 volatile *) (addr + 4)));
    }
}

static inline void xmc_segment_write(u8 index, u32 lval, u32 hval) {
    u32 volatile *l;
    u32 volatile *h;
    l = (u32 volatile *) (XMC_MPAXL0 + index * 8);
    h = (u32 volatile *) (XMC_MPAXH0 + index * 8);
    *l = lval;
    *h = hval;
}

static inline void xmc_segment_map(u8 index, u32 start_addr, u32 segment_size, u32 perm) {
    xmc_segment_write(index, ((start_addr >> 4) & ~0xff) | perm, (start_addr & ~0xfff) | segment_size);
}

static inline void xmc_segment_remap(u8 index, u32 baddr, u32 raddr, u32 segment_size, u32 perm) {
    xmc_segment_write(index, ((raddr >> 4) & ~0xff) | perm, (baddr & ~0xfff) | segment_size);
}

void xmc_init() {
    u32 i;
    /* these settings are based on linker.cmd */

    if (core_id == 0) {
        for (i = 0; i < 0x40000; i += 4) {
            u32 volatile *src = (u32 volatile *) (0x95100000 + i);
            u32 volatile *dst = (u32 volatile *) (0x95140000 + i);
            *dst = *src;
        }
        xmc_segment_remap(2, 0x95100000, 0x95140000, XMC_SEGMENT_SIZE_256K, XMC_SEGMENT_PERM_KERN_RWX_USER____);

        for (i = 0; i < XMC_SEGMENT_NUM; i++) {
            xmc_indices[i] = XMC_INDEX_FREE;
        }
    }

    // default map: kernel only
    xmc_segment_map(0, 0x00000000, XMC_SEGMENT_SIZE_4G, XMC_SEGMENT_PERM_KERN_RWX_USER____);
    // partition share map
    xmc_segment_map(1, 0x95200000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_KERN_RWX_USER_R_X);

}

static inline u32 xmc_size2segment_size(u32 size) {
    if (size == 0x100000) {
        return XMC_SEGMENT_SIZE_1M;
    } else {
        printf("xmc_size2segment_size: size %x\n", size);
        panic("Not supported segment size");
    }
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

    xmc_values_l[index] = ((layout->address >> 4) & ~0xff) | XMC_SEGMENT_PERM_KERN_RWX_USER_RWX;
    xmc_values_h[index] = (layout->address & ~0xfff) | xmc_size2segment_size(layout->size);

    return index;
}

void xmc_segment_activate(u8 index) {
    if (index >= XMC_SEGMENT_NUM || xmc_indices[index] != XMC_INDEX_PART) {
        panic("xmc_segment_activate: Illegal xmc index\n");
    }
    xmc_segment_write(3, xmc_values_l[index], xmc_values_h[index]);
    xmc_invalidate_buffer();
}

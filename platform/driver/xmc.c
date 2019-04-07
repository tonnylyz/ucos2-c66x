#include <printf.h>
#include <partition_conf.h>
#include <os_cpu.h>
#include "xmc.h"
#include "mmio.h"

#define XMC_XPFCMD (0x08000300)
#define XMC_XPFACS (0x08000304)


#define XMC_SEGMENT_SIZE_256K   0b10001
#define XMC_SEGMENT_SIZE_512K   0b10010
#define XMC_SEGMENT_SIZE_1M     0b10011
#define XMC_SEGMENT_SIZE_16M    0b10111
#define XMC_SEGMENT_SIZE_2G     0b11110
#define XMC_SEGMENT_SIZE_4G     0b11111

/*                                               K  U
 *                                              RWXRWX*/
#define XMC_SEGMENT_PERM_KERN_RWX_USER____    0b111000u
#define XMC_SEGMENT_PERM_KERN_RWX_USER_RWX    0b111111u
#define XMC_SEGMENT_PERM_KERN_RWX_USER_R_X    0b111101u
#define XMC_SEGMENT_PERM_KERN_RWX_USER_R__    0b111100u
#define XMC_SEGMENT_PERM_KERN_RWX_USER_RW_    0b111110u

#pragma SET_DATA_SECTION(".data:KERN_SHARE")
static bool xmc_free_id[XMC_INDEX_NUM];
static u32 xmc_values_l[XMC_INDEX_NUM];
static u32 xmc_values_h[XMC_INDEX_NUM];

void xmc_invalidate_buffer(void) {
    barrier();
    *(u32 volatile *)XMC_XPFCMD = 0b11111;
    mmio_read(XMC_XPFACS);
    barrier();
}

static inline void xmc_segment_write(u8 index, u32 lval, u32 hval) {
    u32 volatile *l;
    u32 volatile *h;
    l = (u32 volatile *) (XMC_MPAXL0 + index * 8u);
    h = (u32 volatile *) (XMC_MPAXH0 + index * 8u);
    *l = lval;
    *h = hval;
}

static inline void xmc_segment_map(u8 index, u32 start_addr, u32 segment_size, u32 perm) {
    xmc_segment_write(index, ((start_addr >> 4u) & ~0xffu) | perm, (start_addr & ~0xfffu) | segment_size);
}

static inline void xmc_segment_remap(u8 index, u32 baddr, u32 raddr, u32 segment_size, u32 perm) {
    xmc_segment_write(index, ((raddr >> 4u) & ~0xffu) | perm, (baddr & ~0xfffu) | segment_size);
}

void xmc_init(void) {
    u32 i;
    /* these settings are based on linker.cmd */

    if (core_id == 0) {
        for (i = 0; i < 0x40000; i += 4) {
            u32 volatile *src = (u32 volatile *) (0x95100000 + i);
            u32 volatile *dst = (u32 volatile *) (0x95140000 + i);
            *dst = *src;
        }
        xmc_segment_remap(2, 0x95100000, 0x95140000, XMC_SEGMENT_SIZE_256K, XMC_SEGMENT_PERM_KERN_RWX_USER____);

        for (i = 0; i < XMC_INDEX_NUM; i++) {
            xmc_free_id[i] = true;
        }
    }

    // default map: kernel only
    // TODO: change to USER____ cause XMC exception
    xmc_segment_map(0, 0x00000000, XMC_SEGMENT_SIZE_4G, XMC_SEGMENT_PERM_KERN_RWX_USER_R_X);
    // partition share map
    xmc_segment_map(1, 0x95200000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_KERN_RWX_USER_R_X);

}

static inline u32 xmc_size2segment_size(u32 size) {
    if (size == 0x100000) {
        return XMC_SEGMENT_SIZE_1M;
    } else if (size == 0x80000) {
        return XMC_SEGMENT_SIZE_512K;
    } else if (size == 0x40000) {
        return XMC_SEGMENT_SIZE_256K;
    }
    /* Note:
     *  If other granularity is needed, add here
     * */
    return 0;
}

static u8 _find_free_index() {
    u8 i;
    for (i = 0; i < XMC_INDEX_NUM; i++) {
        if (xmc_free_id[i]) {
            return i;
        }
    }
    return i;
}

u8 xmc_segment_allocate(u32 addr, u32 size) {
    u8 index;
    /* Note:
     *  Core mutex is NOT needed here.
     *  Only core_0 will invoke this function
     * */
    index = _find_free_index();
    xmc_free_id[index] = false;
    xmc_values_l[index] = ((addr >> 4u) & ~0xffu) | XMC_SEGMENT_PERM_KERN_RWX_USER_RWX;
    xmc_values_h[index] = (addr & ~0xfffu) | xmc_size2segment_size(size);
    return index;
}

void xmc_segment_activate(u8 index) {
    xmc_segment_write(3, xmc_values_l[index], xmc_values_h[index]);
    xmc_invalidate_buffer();
}

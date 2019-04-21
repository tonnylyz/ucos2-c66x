#include <printf.h>
#include <os_cpu.h>
#include "xmc.h"
#include "mmio.h"

//#define DISABLE_XMC_PROTECTION

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

void xmc_init(void) {
    /* these settings are based on linker.cmd */
    xmc_segment_map(0, 0x00000000, XMC_SEGMENT_SIZE_4G, XMC_SEGMENT_PERM_KERN_RWX_USER____);
    // task read only  (user_ro)
    xmc_segment_map(1, 0x95200000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_KERN_RWX_USER_R_X);
    // task read/write (user_elf)
    xmc_segment_map(2, 0x95300000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_KERN_RWX_USER_RWX);
    // task read/write (user_rw)
    xmc_segment_map(3, 0x95500000, XMC_SEGMENT_SIZE_1M, XMC_SEGMENT_PERM_KERN_RWX_USER_RWX);
}

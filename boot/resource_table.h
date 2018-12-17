#ifndef _RSC_TABLE_VAYU_DSP_H_
#define _RSC_TABLE_VAYU_DSP_H_

typedef unsigned int UInt32;
typedef char Char;

#define offsetof(st, m) ((UInt32)&(((st *)0)->m))

/* Size constants must match those used on host: include/asm-generic/sizes.h */
#define SZ_64K                          0x00010000
#define SZ_128K                         0x00020000
#define SZ_256K                         0x00040000
#define SZ_512K                         0x00080000
#define SZ_1M                           0x00100000
#define SZ_2M                           0x00200000
#define SZ_4M                           0x00400000
#define SZ_8M                           0x00800000
#define SZ_16M                          0x01000000
#define SZ_32M                          0x02000000
#define SZ_64M                          0x04000000
#define SZ_128M                         0x08000000
#define SZ_256M                         0x10000000
#define SZ_512M                         0x20000000

/* Resource info: Must match include/linux/remoteproc.h: */
#define TYPE_CARVEOUT    0
#define TYPE_DEVMEM      1
#define TYPE_TRACE       2
#define TYPE_VDEV        3

/* Common Resource Structure Types */
struct resource_table {
    UInt32 ver;
    UInt32 num;
    UInt32 reserved[2];
};

struct fw_rsc_carveout {
    UInt32  type;
    UInt32  da;
    UInt32  pa;
    UInt32  len;
    UInt32  flags;
    UInt32  reserved;
    Char    name[32];
};

struct fw_rsc_devmem {
    UInt32  type;
    UInt32  da;
    UInt32  pa;
    UInt32  len;
    UInt32  flags;
    UInt32  reserved;
    Char    name[32];
};

/* DSP Memory Map */
#define L4_PERIPHERAL_L4CFG     0x4A000000
#define DSP_PERIPHERAL_L4CFG    0x4A000000

#define L4_PERIPHERAL_L4PER1    0x48000000
#define DSP_PERIPHERAL_L4PER1   0x48000000

#define L4_PERIPHERAL_L4PER2    0x48400000
#define DSP_PERIPHERAL_L4PER2   0x48400000

#define L4_PERIPHERAL_L4PER3    0x48800000
#define DSP_PERIPHERAL_L4PER3   0x48800000

#define DSP_MEM_TEXT            0x95000000
#define DSP_MEM_DATA            0x95100000

#define DSP_MEM_TEXT_SIZE       SZ_1M
#define DSP_MEM_DATA_SIZE       SZ_1M
/*
 * Assign fixed RAM addresses to facilitate a fixed MMU table.
 */

//BWC
#define VAYU_DSP_1

/* See CMA BASE addresses in Linux side: arch/arm/mach-omap2/remoteproc.c */
#if defined (VAYU_DSP_1)
#define PHYS_MEM_IPC_VRING      0x99000000
#elif defined (VAYU_DSP_2)
#define PHYS_MEM_IPC_VRING      0x9F000000
#endif

/* Need to be identical to that of IPU */
#define PHYS_MEM_IOBUFS         0xBA300000

struct my_resource_table {
    struct resource_table base;
    UInt32 offset[6];  /* Should match 'num' in actual definition */
    struct fw_rsc_carveout text_cout;
    struct fw_rsc_carveout data_cout;
    struct fw_rsc_devmem devmem0;
    struct fw_rsc_devmem devmem1;
    struct fw_rsc_devmem devmem2;
    struct fw_rsc_devmem devmem3;
};

#pragma DATA_SECTION(ti_ipc_remoteproc_ResourceTable, ".resource_table")
#pragma DATA_ALIGN(ti_ipc_remoteproc_ResourceTable, 4096)

struct my_resource_table ti_ipc_remoteproc_ResourceTable = {
    1,      /* we're the first version that implements this */
    6,     /* number of entries in the table */
    0, 0,   /* reserved, must be zero */
    /* offsets to entries */
    {
        offsetof(struct my_resource_table, text_cout),
        offsetof(struct my_resource_table, data_cout),
        offsetof(struct my_resource_table, devmem0),
        offsetof(struct my_resource_table, devmem1),
        offsetof(struct my_resource_table, devmem2),
        offsetof(struct my_resource_table, devmem3)
    },

    {
        TYPE_CARVEOUT,
        DSP_MEM_TEXT, 0,
        DSP_MEM_TEXT_SIZE, 0, 0, "DSP_MEM_TEXT",
    },

    {
        TYPE_CARVEOUT,
        DSP_MEM_DATA, 0,
        DSP_MEM_DATA_SIZE, 0, 0, "DSP_MEM_DATA",
    },

    {
        TYPE_DEVMEM,
        DSP_PERIPHERAL_L4CFG, L4_PERIPHERAL_L4CFG,
        SZ_16M, 0, 0, "DSP_PERIPHERAL_L4CFG",
    },

    {
        TYPE_DEVMEM,
        DSP_PERIPHERAL_L4PER1, L4_PERIPHERAL_L4PER1,
        SZ_2M, 0, 0, "DSP_PERIPHERAL_L4PER1",
    },

    {
        TYPE_DEVMEM,
        DSP_PERIPHERAL_L4PER2, L4_PERIPHERAL_L4PER2,
        SZ_4M, 0, 0, "DSP_PERIPHERAL_L4PER2",
    },

    {
        TYPE_DEVMEM,
        DSP_PERIPHERAL_L4PER3, L4_PERIPHERAL_L4PER3,
        SZ_8M, 0, 0, "DSP_PERIPHERAL_L4PER3",
    }
};

#endif /* _RSC_TABLE_VAYU_DSP_H_ */

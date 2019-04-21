#include "mbr.h"
#include <types.h>
#include <printf.h>


#define le2(a, o) ((a)[o] << 0 | (a)[(o)+1] << 8)
#define le4(a, o) (le2(a,o) | (a)[(o)+2] << 16 | (a)[(o)+3] << 24)
#define lengthof(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
    int cylinder;
    int head;
    int sector;
} chs_t;

typedef struct {
    int status;
    chs_t first_sector;
    int partition_type;
    chs_t last_sector;
    unsigned int first_sector_lba;
    unsigned int sectors;
} mbr_partition_t;

typedef struct {
    char code[440];
    int disk_signature;
    int unused;
    mbr_partition_t partition[4];
    int mbr_signature;
} mbr_t;


static mbr_t mbr;

void mbr_init(void *sector) {
    u8 i;
    unsigned char *mbr_buf = sector;
    _memcpy(mbr.code, mbr_buf, sizeof(mbr.code));
    mbr.disk_signature = le4(mbr_buf, 440);
    mbr.unused = le2(mbr_buf, 444);
    for (i = 0; i < lengthof(mbr.partition); i++) {
        int po = 446 + i * 16;
        mbr_partition_t *p = &mbr.partition[i];
        p->status = mbr_buf[po + 0];
        p->first_sector.head = mbr_buf[po + 1];
        p->first_sector.sector = mbr_buf[po + 2] & 0x3f;
        p->first_sector.cylinder = (mbr_buf[po + 2] & 0xc0) << 2 | mbr_buf[po + 3];
        p->partition_type = mbr_buf[po + 4];
        p->last_sector.head = mbr_buf[po + 5];
        p->last_sector.sector = mbr_buf[po + 6] & 0x3f;
        p->last_sector.cylinder = (mbr_buf[po + 6] & 0xc0) << 2 | mbr_buf[po + 7];
        p->first_sector_lba = le4(mbr_buf, po + 8);
        p->sectors = le4(mbr_buf, po + 12);
    }
    mbr.mbr_signature = le2(mbr_buf, 510);
}

u32 mbr_find_fat32_part() {
    u8 i;
    for (i = 0; i < lengthof(mbr.partition); i++) {
        mbr_partition_t *p = &mbr.partition[i];
        if (p->partition_type == 0x0C) {
            return p->first_sector_lba;
        }
    }
    return 0;
}


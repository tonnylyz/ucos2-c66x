#include "fat32.h"
#include "mbr.h"

#include <ff.h>
#include <mmc.h>
#include <printf.h>

static FATFS fs;
static FRESULT fr;
static FILINFO fi;
static FIL f;

u32 fat32_first_lba;

void fat32_init(void) {
    u8 buf[512];
    mmc_read_sector(0, buf);
    mbr_init(buf);
    fat32_first_lba = mbr_find_fat32_part();
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        panic("f_mount\n");
    }
}

u32 fat32_file_size(char *filename) {
    fr = f_stat(filename, &fi);
    if (fr != FR_OK) {
        return 0;
    }
    return fi.fsize;
}

u32 fat32_read_to_mem(char *filename, u8 *dest, u32 size) {
    u32 actual_read;
    fr = f_open(&f, filename, FA_READ);
    if (fr != FR_OK) {
        return 0;
    }
    fr = f_read(&f, dest, size, &actual_read);
    return actual_read;
}

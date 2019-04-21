#include "mmc.h"
#include <types.h>

#include <ti/csl/csl_mmcsd.h>
#include <ti/board/board.h>
#include <ti/csl/soc.h>
#include <ti/csl/soc/am572x/src/cslr_soc_mpu_baseaddress.h>
#include <ti/csl/soc/am572x/src/cslr_soc_aliases.h>
#include <printf.h>

#include <mmio.h>

static uint32_t baseAddress = CSL_MPU_MMC1_REGS;
#define BUFFER_SIZE    (512)
#define BIT(x) (1 << x)
/* Command/Response flags for notifying some information to controller */
#define SD_CMDRSP_NONE      BIT(0)
#define SD_CMDRSP_STOP      BIT(1)
#define SD_CMDRSP_FS        BIT(2)
#define SD_CMDRSP_ABORT     BIT(3)
#define SD_CMDRSP_BUSY      BIT(4)
#define SD_CMDRSP_136BITS   BIT(5)
#define SD_CMDRSP_DATA      BIT(6)
#define SD_CMDRSP_READ      BIT(7)
#define SD_CMDRSP_WRITE     BIT(8)

/* Check RCA/status */
#define SD_RCA_ADDR(rca)             ((rca & 0xFFFF0000) >> 16)
#define SD_RCA_STAT(rca)             (rca & 0x0xFFFF)
#define MMCSD_IN_FREQ                    (96000000) /* 96MHz */
#define MMCSD_INIT_FREQ                  (400000)   /* 400kHz */

typedef struct _mmcsdCmd {
    unsigned int idx;
    unsigned int flags;
    unsigned int arg;
    signed char *data;
    unsigned int nblks;
    unsigned int rsp[4];
} mmcsdCmd;


uint32_t hsmmcsd_dataLen = 0;
const uint32_t hsmmcsd_blockSize = 512;
volatile uint8_t *hsmmcsd_buffer = 0;

static int32_t HSMMCSDCmdStatusGet(void) {
    int32_t status = STW_SOK;

    while (1) {
        status = HSMMCSDIntrStatusGet(baseAddress, 0xFFFFFFFF);
        if (status & HS_MMCSD_STAT_CMDCOMP) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_CMDCOMP);
            break;
        }
        if (status & HS_MMCSD_STAT_ERR) {
            if (status & HS_MMCSD_STAT_CMDTIMEOUT) {
                HSMMCSDIntrStatusClear(baseAddress,
                                       HS_MMCSD_STAT_CMDTIMEOUT);
                status = STW_ETIMEOUT;
            }
            status = STW_EFAIL;
            break;
        }
    }
    return status;
}

unsigned int HSMMCSDCmdSend(mmcsdCmd *c) {
    unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
    unsigned int dataPresent;
    unsigned int status = STW_SOK;
    unsigned int rspType;
    unsigned int cmdDir;
    unsigned int nblks;
    unsigned int cmd;

    if (c->flags & SD_CMDRSP_STOP) {
        cmdType = HS_MMCSD_CMD_TYPE_SUSPEND;
    } else if (c->flags & SD_CMDRSP_FS) {
        cmdType = HS_MMCSD_CMD_TYPE_FUNCSEL;
    } else if (c->flags & SD_CMDRSP_ABORT) {
        cmdType = HS_MMCSD_CMD_TYPE_ABORT;
    }

    cmdDir = (c->flags & SD_CMDRSP_READ) ? \
             HS_MMCSD_CMD_DIR_READ : HS_MMCSD_CMD_DIR_WRITE;

    dataPresent = (c->flags & SD_CMDRSP_DATA) ? 1 : 0;
    nblks = (dataPresent == 1) ? c->nblks : 0;

    if (c->flags & SD_CMDRSP_NONE) {
        rspType = HS_MMCSD_NO_RESPONSE;
    } else if (c->flags & SD_CMDRSP_136BITS) {
        rspType = HS_MMCSD_136BITS_RESPONSE;
    } else if (c->flags & SD_CMDRSP_BUSY) {
        rspType = HS_MMCSD_48BITS_BUSY_RESPONSE;
    } else {
        rspType = HS_MMCSD_48BITS_RESPONSE;
    }

    cmd = HS_MMCSD_CMD(c->idx, cmdType, rspType, cmdDir);

    if (dataPresent) {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_STAT_TRNFCOMP);

        HSMMCSDDataTimeoutSet(baseAddress, HS_MMCSD_DATA_TIMEOUT(27));
    }

    HSMMCSDCommandSend(baseAddress, cmd, c->arg, (void *) dataPresent,
                       nblks, ((uint32_t) 0));

    HSMMCSDCmdStatusGet();
    if (status == STW_SOK) {
        HSMMCSDResponseGet(baseAddress, (uint32_t *) c->rsp);
    }

    return status;
}

static void HSMMCSDXferSetup(unsigned char rwFlag,
                             void *ptr,
                             unsigned int nBlks) {
    HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_TRNFCOMP);

    if (rwFlag == 1) {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
        HSMMCSDIntrStatusEnable(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
        HSMMCSDIntrStatusDisable(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
    } else {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
        HSMMCSDIntrStatusEnable(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
        HSMMCSDIntrStatusDisable(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
    }

    HSMMCSDBlkLenSet(baseAddress, hsmmcsd_blockSize);
    hsmmcsd_dataLen = (nBlks * hsmmcsd_blockSize);
    hsmmcsd_buffer = (volatile unsigned char *) ptr;
}

static unsigned int HSMMCSDXferStatusGet() {
    volatile uint32_t status = 0;
    int32_t retVal = 0;
    volatile uint32_t i = 0, i_max = 0;
    volatile uint8_t *dst_bfr = hsmmcsd_buffer;
    volatile uint32_t temp;

    while (1) {
        status = HSMMCSDIntrStatusGet(baseAddress, 0xFFFFFFFF);

        if (status & HS_MMCSD_STAT_BUFRDRDY) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_BUFRDRDY);

            if (dst_bfr != NULL) {
                if (hsmmcsd_dataLen < hsmmcsd_blockSize) {
                    i_max = hsmmcsd_dataLen;
                } else {
                    i_max = hsmmcsd_blockSize;
                }
                /*Input data bfr will not be 4-byte aligned*/
                for (i = 0; i < i_max; i += 4) {
                    temp = HW_RD_REG32(baseAddress + MMC_DATA);
                    dst_bfr[i] = *((char *) &temp);
                    dst_bfr[i + 1] = *((char *) &temp + 1);
                    dst_bfr[i + 2] = *((char *) &temp + 2);
                    dst_bfr[i + 3] = *((char *) &temp + 3);
                }
                dst_bfr += i_max;
            }
        }
        if (status & HS_MMCSD_STAT_BUFWRRDY) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_BUFRDRDY);

            if (hsmmcsd_buffer != NULL) {
                for (i = 0; i < hsmmcsd_dataLen; i += 4) {
                    *((char *) &temp) = hsmmcsd_buffer[i];
                    *((char *) &temp + 1) = hsmmcsd_buffer[i + 1];
                    *((char *) &temp + 2) = hsmmcsd_buffer[i + 2];
                    *((char *) &temp + 3) = hsmmcsd_buffer[i + 3];
                    HW_WR_REG32(baseAddress + MMC_DATA, temp);
                }
            }
        }
        if (status & HS_MMCSD_STAT_DATATIMEOUT) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_DATATIMEOUT);
            retVal = STW_ETIMEOUT;
            break;
        }
        if (status & HS_MMCSD_STAT_TRNFCOMP) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_TRNFCOMP);
            retVal = STW_SOK;
            break;
        }
    }
    return retVal;
}

int mmc_read_sector(u32 lba, u8 *buf) {
    int32_t  status = STW_SOK;
    mmcsdCmd cmd;
    HSMMCSDXferSetup(1, buf, 1);
    cmd.idx   = (17);
    cmd.flags = SD_CMDRSP_READ | SD_CMDRSP_DATA;
    cmd.arg   = lba;
    cmd.nblks = 1;
    status    = HSMMCSDCmdSend(&cmd);
    HSMMCSDXferStatusGet();
/*
    printf("mmc_read_sector sector [%08x] ok\n", lba);
    u32 i;
    for (i = 0; i < 32; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 2; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 3; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 4; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 5; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 6; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 7; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 8; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 9; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 10; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 11; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 12; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 13; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 14; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 15; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
    for (; i < 32 * 16; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n=========================\n");*/

    return status;
}

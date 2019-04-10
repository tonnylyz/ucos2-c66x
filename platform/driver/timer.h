#ifndef UCOS2_TIMER_H
#define UCOS2_TIMER_H

#include "types.h"

#define GP_TIMER_1_BASE          (0x4AE18000)
#define GP_TIMER_2_BASE          (0x48032000)
#define GP_TIMER_3_BASE          (0x48034000)
#define GP_TIMER_4_BASE          (0x48036000)
#define GP_TIMER_5_BASE          (0x48820000)
#define GP_TIMER_6_BASE          (0x48822000)
#define GP_TIMER_7_BASE          (0x48824000)
#define GP_TIMER_8_BASE          (0x48826000)
#define GP_TIMER_9_BASE          (0x4803E000)
#define GP_TIMER_10_BASE         (0x48086000)
#define GP_TIMER_11_BASE         (0x48088000)
#define GP_TIMER_12_BASE         (0x4AE20000)
#define GP_TIMER_13_BASE         (0x48828000)
#define GP_TIMER_14_BASE         (0x4882A000)
#define GP_TIMER_15_BASE         (0x4882C000)
#define GP_TIMER_16_BASE         (0x4882E000)

/* Note:
 * Timers use 20 MHz clock source
 * Typical TIMER_FREQUENCY values:
 *  - 1s        interval ~ 0x1312D00
 *  - 100ms     interval ~ 0x1E8480
 *  - 10ms      interval ~ 0x30D40
 *  - 1ms       interval ~ 0x4E20
 * */
#define GP_TIMER_INTERVAL_1S     (0x01312D00)
#define GP_TIMER_INTERVAL_100MS  (0x001E8480)
#define GP_TIMER_INTERVAL_10MS   (0x00030D40)
#define GP_TIMER_INTERVAL_1MS    (0x00004E20)


#define GP_TASK_TIMER_BASE       GP_TIMER_5_BASE
#define GP_PART_TIMER_BASE       GP_TIMER_6_BASE

void timer_init(void);

void timer_irq_clear(u32 base);

void timer_benchmark_restart();

u32 timer_benchmark_stop();

#endif //UCOS2_TIMER_H

#include "stub.h"

u32 t1_stack[512];
u32 t2_stack[512];

void task_entry(void *arg) {
    while (1) {
        stub_putc(*((char *)arg));
        stub_putc('\n');
        stub_time_delay(2);
    }
}

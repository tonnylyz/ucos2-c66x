#include "stub.h"
#include "task_types.h"

void (*stub_putc)(char)  = 0;
void (*stub_time_delay)(u32) = 0;

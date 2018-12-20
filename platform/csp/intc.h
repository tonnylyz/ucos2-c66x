#ifndef UCOS2_C66X_INTC_H
#define UCOS2_C66X_INTC_H

#include <types.h>

void intc_init();

u32 intc_global_nmi_enable();

u32 intc_global_enable(u32 *previous_status);

u32 intc_vector_enable(u32 vectId);

u32 intc_vector_disable(u32 vectId);

void irq_init();

#endif //UCOS2_C66X_INTC_H

#include "intc.h"
#include <types.h>
#include <snprintf.h>
#include <os_cpu.h>
#include <timer.h>
#include <ucos_ii.h>

#include <ti/csl/src/intc/csl_intc.h>
#include <ti/csl/src/intc/csl_intcAux.h>

#define INTC_EVID_TIMER_2 64


CSL_IntcContext             intcContext;
CSL_IntcEventHandlerRecord  EventHandler[30];
CSL_IntcGlobalEnableState   state;

void intc_init() {
    intcContext.eventhandlerRecord = EventHandler;
    intcContext.numEvtEntries      = 10;
    if (CSL_intcInit(&intcContext) != CSL_SOK)
    {
        printf("Error: GEM-INTC initialization failed\n");
        return;
    }

    /* Enable NMIs */
    if (CSL_intcGlobalNmiEnable() != CSL_SOK)
    {
        printf("Error: GEM-INTC global NMI enable failed\n");
        return;
    }

    /* Enable global interrupts */
    if (CSL_intcGlobalEnable(&state) != CSL_SOK)
    {
        printf ("Error: GEM-INTC global enable failed\n");
        return;
    }
}

extern void OSTickISR();

CSL_IntcObj                 intcObj;
CSL_IntcEventHandlerRecord  EventRecord;
CSL_IntcParam               vectId;
CSL_IntcHandle              hTest;

void irq_init() {
    //CSL_intcMapEventVector(INTC_EVID_TIMER_2, CSL_INTC_VECTID_4);
    //CSL_intcInterruptEnable(INTC_EVID_TIMER_2);

    vectId = CSL_INTC_VECTID_4;
    hTest = CSL_intcOpen (&intcObj, 64, &vectId , NULL);
    if (hTest == NULL)
    {
        printf("Error: GEM-INTC Open failed\n");
        return;
    }

    /* Register an call-back handler which is invoked when the event occurs. */
    EventRecord.handler = &OSTickISR;
    EventRecord.arg = 0;
    if (CSL_intcPlugEventHandler(hTest,&EventRecord) != CSL_SOK)
    {
        printf("Error: GEM-INTC Plug event handler failed\n");
        return;
    }

    /* Enabling the events. */
    if (CSL_intcHwControl(hTest,CSL_INTC_CMD_EVTENABLE, NULL) != CSL_SOK)
    {
        printf("Error: GEM-INTC CSL_INTC_CMD_EVTENABLE command failed\n");
        return;
    }

}

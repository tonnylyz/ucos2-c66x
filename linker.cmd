/****************************************************************************/
/*  AM57xx_C66.cmd                                                          */
/*  Copyright (c) 2015  Texas Instruments Incorporated                      */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on the C66x DSP core of  */
/*                 an AM57xx device.                                        */
/*                 Use it as a guideline.  You will want to                 */
/*                 change the memory layout to match your specific          */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/
--heap_size=0x1000
--stack_size=0x10000
--retain="*(.resource_table)"

-lti.csl.ae66
-lti.csl.intc.ae66

MEMORY
{
    L2SRAM (RWX) : org = 0x800000, len = 0x40000
    OCMC_RAM1 (RWX) : org = 0x40300000, len = 0x80000
    OCMC_RAM2 (RWX) : org = 0x40400000, len = 0x100000
    OCMC_RAM3 (RWX) : org = 0x40500000, len = 0x100000
    EXT_CODE (RWX) : org = 0x95000000, len = 0x100000
    EXT_DATA (RW) : org = 0x95100000, len = 0x100000
}

SECTIONS
{
    .text:RESET: load > 0x95000000
    //.text:_c_int00: load > 0x95000000
    .text: load > EXT_CODE
    .stack: load > EXT_DATA
    GROUP: load > EXT_DATA
    {
        .bss:
        .neardata:
        .rodata:
    }
    .cinit: load > EXT_DATA
    .pinit: load > EXT_DATA
    .init_array: load > EXT_DATA
    .const: load > EXT_DATA
    .data: load > EXT_DATA
    .fardata: load > EXT_DATA
    .switch: load > EXT_DATA
    .sysmem: load > EXT_DATA
    .far: load > EXT_DATA
    .args: load > EXT_DATA align = 0x4, fill = 0 {_argsize = 0x0; }
    .cio: load > EXT_DATA
    .ti.handler_table: load > EXT_DATA
    .c6xabi.exidx: load > EXT_DATA
    .c6xabi.extab: load > EXT_DATA
    .resource_table: load > EXT_CODE, type = NOINIT
    .vecs: load > EXT_CODE
    .csl_vect: load > EXT_CODE
}



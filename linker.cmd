--heap_size=0x0
--stack_size=0x10000

//-lti.csl.ae66
//-lti.csl.intc.ae66

MEMORY
{
    KERN_TEXT (RX)  : org = 0x95000000, len = 0x100000
    KERN_DATA (RWX) : org = 0x95100000, len = 0x100000
    USER_RO   (RX)  : org = 0x95200000, len = 0x100000
    USER_RW   (RWX) : org = 0x95300000, len = 0x100000
    KERN_STACK(RWX) : org = 0x95400000, len = 0x100000
}

SECTIONS
{
    .text:VECTOR: load > 0x95000000
    .text: load > KERN_TEXT

    .stack: load > KERN_STACK
    GROUP: load > KERN_DATA
    {
        .bss:
        .rodata:
    }
    .cinit: load > KERN_DATA
    .init_array: load > KERN_DATA
    .data: load > KERN_DATA
    .fardata: load > KERN_DATA
    .switch: load > KERN_DATA
    .far: load > KERN_DATA

    .const: load > USER_RO
    .text:USER: load > USER_RO
    .data:USER: load > USER_RW
}



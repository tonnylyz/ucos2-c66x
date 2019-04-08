--heap_size=0x0
--stack_size=0x10000

MEMORY
{
    KERN_TEXT    (RWX) : org = 0x95000000, len = 0x100000
    KERN_DATA    (RW)  : org = 0x95100000, len = 0x40000
    KERN_DATA2   (RW)  : org = 0x95140000, len = 0x40000
    KERN_SHARE   (RW)  : org = 0x95180000, len = 0x80000

    PART_S (RWX) : org = 0x95200000, len = 0x100000
    PART_0 (RWX) : org = 0x95300000, len = 0x100000
    PART_1 (RWX) : org = 0x95400000, len = 0x100000
    PART_2 (RWX) : org = 0x95500000, len = 0x100000
    PART_3 (RWX) : org = 0x95600000, len = 0x100000
}

SECTIONS
{
    .text:VECTOR: load > 0x95000000
    .text: load > KERN_TEXT

    .stack: load > KERN_DATA
    GROUP: load > KERN_DATA
    {
        .bss:
        .rodata:
    }
    .cinit: load > KERN_DATA
    .init_array: load > KERN_DATA
    .data: load > KERN_DATA
    .neardata: load > KERN_DATA
    .fardata: load > KERN_DATA
    .switch: load > KERN_DATA
    .far: load > KERN_DATA

    .const: load > KERN_SHARE
    .data:KERN_SHARE: load > KERN_SHARE

    .text:PART_S: load > PART_S
    .data:PART_S: load > PART_S
    .const:.string: load > PART_S

    .text:PART_0: load > PART_0
    .data:PART_0: load > PART_0
    .text:PART_1: load > PART_1
    .data:PART_1: load > PART_1
    .text:PART_2: load > PART_2
    .data:PART_2: load > PART_2
    .text:PART_3: load > PART_3
    .data:PART_3: load > PART_3
}



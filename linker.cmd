--heap_size=0x0
--stack_size=0x10000
--retain="*(.resource_table)"

MEMORY
{
    KERN_TEXT (RWX) : org = 0x95000000, len = 0x100000
    KERN_DATA (RW)  : org = 0x95100000, len = 0x100000

    PART_SHARE_TEXT (RWX) : org = 0x95200000, len = 0x80000
    PART_SHARE_DATA (RWX) : org = 0x95280000, len = 0x80000

    PART_TEXT_0 (RW)  : org = 0x95300000, len = 0x80000
    PART_DATA_0 (RWX) : org = 0x95380000, len = 0x80000
    PART_TEXT_1 (RW)  : org = 0x95400000, len = 0x80000
    PART_DATA_1 (RWX) : org = 0x95480000, len = 0x80000
    PART_TEXT_2 (RW)  : org = 0x95500000, len = 0x80000
    PART_DATA_2 (RWX) : org = 0x95580000, len = 0x80000
    PART_TEXT_3 (RW)  : org = 0x95600000, len = 0x80000
    PART_DATA_3 (RWX) : org = 0x95680000, len = 0x80000
    PART_TEXT_4 (RW)  : org = 0x95700000, len = 0x80000
    PART_DATA_4 (RWX) : org = 0x95780000, len = 0x80000
    PART_TEXT_5 (RW)  : org = 0x95800000, len = 0x80000
    PART_DATA_5 (RWX) : org = 0x95880000, len = 0x80000
    PART_TEXT_6 (RW)  : org = 0x95900000, len = 0x80000
    PART_DATA_6 (RWX) : org = 0x95980000, len = 0x80000
    PART_TEXT_7 (RW)  : org = 0x95a00000, len = 0x80000
    PART_DATA_7 (RWX) : org = 0x95a80000, len = 0x80000
}

SECTIONS
{
    .text:VECTOR: load > 0x95000000
    .text: load > KERN_TEXT

    .stack: load > KERN_DATA
    GROUP: load > KERN_DATA
    {
        .bss:
        .neardata:
        .rodata:
    }
    .cinit: load > KERN_DATA
    .init_array: load > KERN_DATA
    .const: load > KERN_DATA
    .data: load > KERN_DATA
    .fardata: load > KERN_DATA
    .switch: load > KERN_DATA
    .far: load > KERN_DATA
    .resource_table: load > KERN_TEXT, type = NOINIT

    .text:PART_S: load > PART_SHARE_TEXT
    .data:PART_S: load > PART_SHARE_DATA

    .text:PART_0: load > PART_TEXT_0
    .data:PART_0: load > PART_DATA_0
    .text:PART_1: load > PART_TEXT_1
    .data:PART_1: load > PART_DATA_1
    .text:PART_2: load > PART_TEXT_2
    .data:PART_2: load > PART_DATA_2
    .text:PART_3: load > PART_TEXT_3
    .data:PART_3: load > PART_DATA_3
    .text:PART_4: load > PART_TEXT_4
    .data:PART_4: load > PART_DATA_4
    .text:PART_5: load > PART_TEXT_5
    .data:PART_5: load > PART_DATA_5
    .text:PART_6: load > PART_TEXT_6
    .data:PART_6: load > PART_DATA_6
    .text:PART_7: load > PART_TEXT_7
    .data:PART_7: load > PART_DATA_7
}



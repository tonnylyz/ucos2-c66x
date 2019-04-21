--heap_size=0
--stack_size=0
-e=task_entry
--retain=t1_stack
--retain=t2_stack

MEMORY
{
    USER_RW   (RWX) : org = 0x95300000, len = 0x100000
}

SECTIONS
{
    .text: load > USER_RW
    .stack: load > USER_RW
    .bss: load > USER_RW
    .rodata: load > USER_RW
    .cinit: load > USER_RW
    .init_array: load > USER_RW
    .data: load > USER_RW
    .fardata: load > USER_RW
    .neardata: load > USER_RW
    .switch: load > USER_RW
    .far: load > USER_RW
    .const: load > USER_RW
}



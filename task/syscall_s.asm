    .sect ".text:PART_S"
    .global task_syscall
task_syscall:
    SWE
    NOP     5
    BNOP    B3,5

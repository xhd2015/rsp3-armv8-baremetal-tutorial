__asm__(
    ".section .text.boot \n\t"
    "ldr     x0,=0x3F215040 \n\t"
    "mov     x1,'A'       \n\t"
    "1: \n\t"
    "strh     w1,[x0]   \n\t"
    "b       1b   \n\t"
);

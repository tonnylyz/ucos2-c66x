#include "uart.h"
#include <mmio.h>

#define UC_UART_THR(base)  (base + 0x00)
#define UC_UART_LSR(base)  (base + 0x14)

#define UART_BASE_1 (0x4806a000U)
#define UART_BASE_2 (0x4806c000U)
#define UART_BASE_3 (0x48020000U)
#define UART_BASE_8 (0x48422000U)

static void _uart_putc(u32 base, char c) {
    while ((mmio_readb(UC_UART_LSR(base)) & 0x20u) == 0);
    mmio_writeb(UC_UART_THR(base), (u8)c);
}

void uart_putc(char c) {
    if (c == '\n') {
        _uart_putc(UART_BASE_1, '\r');
    }
    _uart_putc(UART_BASE_1, c);
}

void uart_puts(char *str) {
    char c;

    while (0 != (c = *str++)) {
        uart_putc(c);
    }
}

#include "resource_table.h"
#include "types.h"

#define UC_UART_THR(base)  (base + 0x00)
#define UC_UART_LSR(base)  (base + 0x14)

#define UART_BASE_1 (0x4806a000U)
#define UART_BASE_2 (0x4806c000U)
#define UART_BASE_3 (0x48020000U)
#define UART_BASE_8 (0x48422000U)

static inline u8 mmio_readb(u32 ptr) {
    return *((u8 *)ptr);
}

static inline void mmio_writeb(u32 ptr, u8 value) {
    *((u8 *)ptr) = value;
}

static inline void _uart_putc(u32 base, char c) {
    while ((mmio_readb(UC_UART_LSR(base)) & 0x20) == 0);
    mmio_writeb(UC_UART_THR(base), (u8)c);
}

static inline void uart_putc(char c) {
    if (c == '\n') {
        _uart_putc(UART_BASE_3, '\r');
    }
    _uart_putc(UART_BASE_3, c);
}

static inline void uart_puts(char *str) {
    char c;

    while (0 != (c = *str++)) {
        uart_putc(c);
    }
}

int main() {
    while (1) {
        uart_puts("hello world!\n");
    }
}

#ifndef UART_H
#define UART_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>

// FIFO dispatcher data type
struct data_t {
    void *fifo_reserved;
    char msg[20];
};

extern struct k_fifo dispatcher_fifo;

int init_uart(void);
void uart_task(void *, void *, void *);

#endif
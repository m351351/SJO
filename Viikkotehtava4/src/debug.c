#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "debug.h"

K_FIFO_DEFINE(data_fifo);

void debug_task(void *unused1, void *unused2, void *unused3)
{
    struct debug_data_t *received;

    while (true) {
        received = k_fifo_get(&data_fifo, K_FOREVER);
        printk("Debug received time: %lld ns\n", received->time);
        k_free(received);

        k_yield();
    }
}
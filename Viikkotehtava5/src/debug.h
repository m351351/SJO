#ifndef DEBUG_H
#define DEBUG_H

#include <zephyr/kernel.h>

struct debug_data_t {
    void *fifo_reserved;
    uint64_t time;
};

extern struct k_fifo data_fifo;
extern bool debug_enabled;

void debug_task(void *, void *, void *);

#endif
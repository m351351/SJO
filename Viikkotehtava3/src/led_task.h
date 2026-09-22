#ifndef LED_TASK_H
#define LED_TASK_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

extern struct k_mutex val_mutex;
extern struct k_condvar red_cv;
extern struct k_condvar yellow_cv;
extern struct k_condvar green_cv;
extern struct k_sem release_sem;

extern char active_color;
extern int active_duration;

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;


int init_led(void);

void red_led_task(void*, void*, void*);
void yellow_led_task(void*, void*, void*);
void green_led_task(void*, void*, void*);

#endif
#ifndef BUTTON_H
#define BUTTON_H

#include <zephyr/drivers/gpio.h>

int init_button();

extern volatile int tilakone;
extern int saved_state;

// Ulkoiset viittaukset ledeihin
extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;
extern const struct gpio_dt_spec blue;
extern const struct gpio_dt_spec yellow;

#endif
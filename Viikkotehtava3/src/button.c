#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include "button.h"

#define BUTTON_0 DT_ALIAS(sw0)
#define BUTTON_1 DT_ALIAS(sw1)
#define BUTTON_2 DT_ALIAS(sw2)
#define BUTTON_3 DT_ALIAS(sw3)
#define BUTTON_4 DT_ALIAS(sw4)

static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});
static struct gpio_callback button_0_data;
static struct gpio_callback button_1_data;
static struct gpio_callback button_2_data;
static struct gpio_callback button_3_data;
static struct gpio_callback button_4_data;

int saved_state = 0;
//int tilakone = 0;

void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 0 PAUSE pressed\n");
    if (tilakone != 4) {
        saved_state = tilakone; 
        tilakone = 4;           
    } else {
        tilakone = saved_state; 
    }
}

static int tsekkaa_nappi (const struct gpio_dt_spec *button, struct gpio_callback *callback, gpio_callback_handler_t handler)
{
	if (!device_is_ready(button->port)) {
		printk("Error: button device %s is not ready\n", button->port->name);
		return -ENODEV;
	}

	// LISÄTTY GPIO_PULL_UP, jotta pinni ei leiju!
	int ret = gpio_pin_configure_dt(button, GPIO_INPUT | GPIO_PULL_UP);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button->port->name, button->pin);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
		       ret, button->port->name, button->pin);
		return ret;
	}

	gpio_init_callback(callback, handler, BIT(button->pin));
	gpio_add_callback(button->port, callback);
	printk("Set up button at %s pin %d\n", button->port->name, button->pin);

	return 0;
}

int init_button() {
	
	tsekkaa_nappi(&button_0, &button_0_data, button_0_handler);
	
	return 0;
}
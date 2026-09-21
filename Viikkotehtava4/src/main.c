// Meri-Tuulia Turtinen
// TVT24SPL
// Tavoitellaan kolmea pistettä
// Nyt tehty ajan laskenta noihin tehtäviin ja lisätty kuvat omaan kansioon "kuvat"
// debugille tehty oma moduuli, jolloin viestit menevät erillisen debug taskin kautta
// debugin saa päälle ja pois painamalla d kirjainta


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/timing/timing.h>
#include "button.h"
#include "uart.h"
#include "dispatcher.h"
#include "debug.h"

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5

const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec yellow = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);


volatile int tilakone = 0; 
int led_state = 0;

int init_led() {
    int ret;

    ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) return ret;
    gpio_pin_set_dt(&red, 0);

    ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) return ret;
    gpio_pin_set_dt(&green, 0);

    printk("Led initialized ok\n");
    return 0;
}

int main(void)
{
    init_button();

    printk("Dispatcher example started\n");
    int ret = init_uart();
    if (ret != 0) {
        printk("UART initialization failed!\n");
        return ret;
    }
    
    ret = init_led();
    if (ret != 0) {
        printk("Led initialization failed!\n");
        return ret;
    }

    timing_init();
    timing_start();
    timing_t start_time = timing_counter_get();

    k_msleep(100);

    printk("Program started..\n");

    timing_t end_time = timing_counter_get();
    timing_stop();
    uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
    printk("Initialization: %lld\n", timing_ns);

    /* 
    while (true) {
        printk("MAIN TASKI\n");
        k_msleep(100);
    }*/

    k_yield();

    
    return 0;
}

// Säikeiden määritykset
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(debug_thread, STACKSIZE, debug_task, NULL, NULL, NULL, PRIORITY, 0, 0);
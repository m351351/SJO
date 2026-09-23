// Meri-Tuulia Turtinen
// TVT24SPL
// Tavoitellaan kolmea pistettä
//


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/timing/timing.h>
#include "button.h"
#include "uart.h"
#include "dispatcher.h"
#include "debug.h"
#include "led_task.h"
#include <ctype.h>

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5


volatile int tilakone = 0; 
int led_state = 0;
struct k_timer timer;
struct k_timer timer2;

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

    k_mutex_init(&val_mutex);
    k_condvar_init(&red_cv);
    k_condvar_init(&yellow_cv);
    k_condvar_init(&green_cv);
    k_sem_init(&release_sem, 0, 1);

    timing_init();
    timing_start();
    timing_t start_time = timing_counter_get();

    k_msleep(100);

    printk("Program started..\n");

    timing_t end_time = timing_counter_get();
    timing_stop();
    uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
    printk("Initialization: %lld\n", timing_ns);

    k_yield();

     while (true) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
	



// Säikeiden määritykset
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(debug_thread, STACKSIZE, debug_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(red_thread, STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(yellow_thread, STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(green_thread, STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
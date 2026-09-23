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

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5


volatile int tilakone = 0; 
int led_state = 0;

#define COMMAND_OK 0
//definaa tänne ne samat virhekoodit mitä oli TimeParser.h:ssa
int parser(char *command); // tämä muokkaa oikeannimiseksi sitten



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

    //tänne niitä ehtoja noista palautusarvoista. 
    // jos ret on pienempi kuin nolla --> timevalueerror
    // jos isompi ku nolla --> command ok
    // jne.
    char c=0;
	int cnt = 0;
	char buffer[16];

	// superloop
	while (true) {

		if (uart_poll_in(uart_dev,&c) == 0) {
			if (c == '\n' || c == '\r') {
				// printk(buffer);
				// here you call parser
				int ret = parser(buffer);
				// check parser return value
				if (ret == COMMAND_OK) {
					// send signal / message to mailbox
                    // tänne keskeytystimer teemun ohjeista debug: k_time_init
                    // Timer initialization
                    k_timer_init(&timer, timer_handler, NULL);
                    k_timer_start(&timer, K_SECONDS(ret), NULL);
				} 
				// clear buffer and counter
				cnt = 0;
				memset(buffer,0,16);
			} else {
				// add received character to buffer
				buffer[cnt] = c;
				cnt++;
			}
		}
	}

    
    return 0;
}

// alla olevaan kopioidaan se koodia TimeParser.cpp. Eli tuo alla oleva pois
// ja se toinne tilalle
int parser(char *command) {
	int ret = COMMAND_OK;

	// add your own code from googletest here!!
	// to check the buffer for correct sequence
	
	return ret;
}

//tämä on vika vaihe vk5 tehtävässä. Voi laittaa hommaksi mitä vaan
// control led using timer interrupt
void timer_handler(struct k_timer *timer_id) {
	if (red_state == false) {
		gpio_pin_set_dt(&red,1);
		red_state = true;
	} else {
		gpio_pin_set_dt(&red,0);
		red_state = false;
	}
}

// Säikeiden määritykset
K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(debug_thread, STACKSIZE, debug_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(red_thread, STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(yellow_thread, STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(green_thread, STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
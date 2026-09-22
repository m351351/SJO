// Meri-Tuulia Turtinen
// TVT24SPL
// Tavoite kolme pistettä
// Yhden pisteen tehtävä suoritettu
// en ole edes käyttänyt while-looppeja ledeihin joten ilmeisesti sekin ok eli kaksi pistettä
// T laskuri lisätty eli KOLME PISTETTÄ
// start terminal with new configuration


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <stdlib.h>
#include "led_task.h"



/****************************
 * Remember to add line:
 * CONFIG_HEAP_MEM_POOL_SIZE=1024
 * to prj.conf
 ****************************/

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5

// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);


// Create dispatcher FIFO buffer
K_FIFO_DEFINE(dispatcher_fifo);

//void red_led_task(void *, void *, void*);
//void green_led_task(void *, void *, void*);
//void yellow_led_task(void *, void *, void*);

// FIFO dispatcher data type
struct data_t {
	/*************************
	// Add fifo_reserved below
	*************************/
	void *fifo_reserved;
	char msg[20];
};


/********************
 * init UART
 */
int init_uart(void) {
	// UART initialization
	if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
		return 1;
	} 
	return 0;
}

/********************
 * Main task
 */
int main(void)
{
    printk("Dispatcher example started\n");

	k_mutex_init(&val_mutex);
	k_condvar_init(&red_cv);
	k_condvar_init(&green_cv);
	k_condvar_init(&yellow_cv);
	k_sem_init(&release_sem, 0,1);

	int ret = init_uart();
    
	if (ret != 0) {
		printk("UART initialization failed!\n");
		return ret;
	}
    
    init_led();
    if (ret != 0) {
		printk("Led initialization failed!\n");
		return ret;
	}

	return 0;
}

/********************
 * UART task
 */
static void uart_task(void *unused1, void *unused2, void *unused3)
{
	// Received character from UART
	char rc=0;
	// Message from UART
	char uart_msg[20];
	memset(uart_msg,0,20);
	int uart_msg_cnt = 0;

	while (true) {
		// Ask UART if data available
		if (uart_poll_in(uart_dev,&rc) == 0) {
			// printk("Received: %c\n",rc);
			// If character is not newline, add to UART message buffer
			if (rc != '\r') {
				uart_msg[uart_msg_cnt] = rc;
				uart_msg_cnt++;
			// Character is newline, copy dispatcher data and put to FIFO buffer
			} else {
				printk("UART msg: %s\n", uart_msg);
                                
                                // FIFO Stuff begins
				
                                struct data_t *buf = k_malloc(sizeof(struct data_t));
				if (buf == NULL) {
					return;
				}
				// Copy UART message to dispatcher data
				// strncpy(buf->msg, 20, uart_msg); // mitä ihmettä, miksi kaatuu!!
				snprintf(buf->msg, 20, "%s", uart_msg);

				// You need to:
				// Put dispatcher data to FIFO buffer
                                k_fifo_put(&dispatcher_fifo, buf);
				// Clear UART receive buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,20);

				// Clear UART message buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,20);
			}
		}
		k_msleep(10);
                //return 0;
	}
	
}


/********************
 * Dispatcher task
 */
static void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
	while (true) {
		// Receive dispatcher data from uart_task fifo
		struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
		char sequence[20];
		memset(sequence,0,sizeof(sequence));
		strncpy(sequence, rec_item->msg, sizeof(sequence)-1);
		k_free(rec_item);

		printk("Dispatcher: %s\n", sequence);

		int i = 0;
		int Tlaskuri=0;
		while (sequence[i] != '\0')
		{
			char color = sequence[i];
			i++;

			if (color == 'T' || color == 't')
			{
				if (Tlaskuri == 0){
					i = 0;
					printk("UUdestaan");
					Tlaskuri = 1;
					continue;
				}else{
				continue;
			}
			}

			int duration = 1000;
			if (sequence[i]==',')
			{
				i++;
				duration=atoi(&sequence[i]);
				while (sequence[i] >= '0' && sequence[i] <= '9')
				{
					i++;
				}
			}

			k_mutex_lock(&val_mutex, K_FOREVER);
			active_color = color;
			active_duration = duration;

			if (color == 'R' || color == 'r')
			{
				k_condvar_signal(&red_cv);
			}
			else if(color == 'Y' || color == 'y')
			{
				k_condvar_signal(&yellow_cv);
			}
			else if(color == 'G' || color == 'g')
			{
				k_condvar_signal(&green_cv);
			}
			k_mutex_unlock(&val_mutex);
			k_sem_take(&release_sem, K_FOREVER);
		}
		
	}
}

K_THREAD_DEFINE(dis_thread, STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_thread, STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(red_thread, STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(yellow_thread, STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(green_thread, STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);
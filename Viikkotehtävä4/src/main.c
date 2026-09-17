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



/****************************
 * Remember to add line:
 * CONFIG_HEAP_MEM_POOL_SIZE=1024
 * to prj.conf
 ****************************/

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5

const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec yellow = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

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


int init_led() {
	int ret;

	ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
	if (ret < 0){
        printk("olet täällä: 1\n");
		return ret;
	}
	

	ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
	if (ret < 0){
        printk("olet täällä: 2\n");  
        return ret;
    }

	gpio_pin_set_dt(&red, 0);
    gpio_pin_set_dt(&green, 0);
    //gpio_pin_set_dt(&yellow, 0);
	printk("Led initialized ok\n");
	return 0;
}
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
		memcpy(sequence,rec_item->msg,20);
		k_free(rec_item);

		printk("Dispatcher: %s\n", sequence);


        // You need to:
        // Parse color and time from the fifo data
        // Example
		int Tlaskuri = 0;
		
		for (int i =0; sequence[i] != '\0';) {
            char color = sequence[i];
		

        if (color == 'R' || color == 'r') {
            printk("Color: RED\n");
            gpio_pin_set_dt(&red,1);
            k_msleep(1000);
            gpio_pin_set_dt(&red,0);
			i++;
            
        } else if (color == 'Y' || color == 'y') {
            printk("Color: YELLOW\n");
            gpio_pin_set_dt(&red,1);
			gpio_pin_set_dt(&green,1);
            k_msleep(1000);
            gpio_pin_set_dt(&red,0);
			gpio_pin_set_dt(&green,0);
			i++;

        } else if (color == 'G' || color == 'g') {
            printk("Color: GREEN\n");
            gpio_pin_set_dt(&green,1);
            k_msleep(1000);
            gpio_pin_set_dt(&green,0);
			i++;
		}
		else if (color == 'T' || color == 't') {
			if (Tlaskuri == 0){
			i = 0;
			printk("UUdestaan");
			Tlaskuri++;
		}else{
			i++;
		}
		k_msleep(1000);
			

		} else {
            printk("Unknown color: %c\n", color);
        }
        int time = atoi(sequence+2); 
		printk("Data: %c %d\n", color, time);
        // Send the parsed color information to tasks using fifo
        // Use release signal to control sequence or k_yield
	}
	
	}
}

K_THREAD_DEFINE(dis_thread,STACKSIZE,dispatcher_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);

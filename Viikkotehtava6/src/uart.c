
/********************
 * UART task
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include "uart.h"

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

K_FIFO_DEFINE(dispatcher_fifo);

extern int time_parse(char *time);

int init_uart(void) {
    if (!device_is_ready(uart_dev)) {
        //printk("UART device not ready\n");
        return 1;
    } 
    return 0;
}

 void uart_task(void *unused1, void *unused2, void *unused3)
{
	// Received character from UART
	char rc = 0;
	// Message from UART
	char uart_msg[20];
	memset(uart_msg, 0, 20);
	int uart_msg_cnt = 0;

	while (true) {
		// Ask UART if data available
		if (uart_poll_in(uart_dev, &rc) == 0) {
			
			// If character is not X, add to UART message buffer
			if (rc != 'X') {
				uart_msg[uart_msg_cnt] = rc;
				uart_msg_cnt++;
				uart_msg[uart_msg_cnt] = '\0';
			} 
			// Character is X, process the time string and reply
			else {
				int timer_delay = time_parse(uart_msg); 
				
				char response_str[20];
				snprintf(response_str, sizeof(response_str), "%dX", timer_delay);

				for (int k = 0; response_str[k] != '\0'; k++) {
					uart_poll_out(uart_dev, response_str[k]);   
				} 
				
				// Clear UART receive buffer after processing
				uart_msg_cnt = 0;
				memset(uart_msg, 0, 20);
			}
		}
		k_msleep(10);
	}
}
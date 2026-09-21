#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/timing/timing.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include "dispatcher.h"
#include "uart.h"
#include "button.h"
#include "debug.h"

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;

void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
    while (true) {
        // Receive dispatcher data from uart_task fifo
        struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
        char sequence[20];
        memcpy(sequence, rec_item->msg, 20);
        k_free(rec_item);

        uint64_t total_sequence_time = 0;
        int Tlaskuri = 0;
        
        for (int i = 0; sequence[i] != '\0';) {
            char color = sequence[i];

            if (color == 'R' || color == 'r') {
                timing_start();
                timing_t red_start_time = timing_counter_get();

                gpio_pin_set_dt(&red, 1);
                k_msleep(1000);
                gpio_pin_set_dt(&red, 0);
                i++;

                timing_t red_end_time = timing_counter_get();
                timing_stop();
                uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&red_start_time, &red_end_time));
                
				struct debug_data_t *buf = k_malloc(sizeof(struct debug_data_t));
				if (buf != NULL) {
					buf->time = timing_ns;
					k_fifo_put(&data_fifo, buf);
				}
				k_yield();


            } else if (color == 'Y' || color == 'y') {
                timing_start();
                timing_t yellow_start_time = timing_counter_get();

                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 1);
                k_msleep(1000);
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 0);
                i++;

                timing_t yellow_end_time = timing_counter_get();
                timing_stop();
                uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&yellow_start_time, &yellow_end_time));
                //total_sequence_time += timing_ns;

				struct debug_data_t *buf = k_malloc(sizeof(struct debug_data_t));
				if (buf != NULL) {
					buf->time = timing_ns;
					k_fifo_put(&data_fifo, buf);
				}
				k_yield();

            } else if (color == 'G' || color == 'g') {
                timing_start();
                timing_t green_start_time = timing_counter_get();

                gpio_pin_set_dt(&green, 1);
                k_msleep(1000);
                gpio_pin_set_dt(&green, 0);
                i++;

                timing_t green_end_time = timing_counter_get();
                timing_stop();
                uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&green_start_time, &green_end_time));
                //total_sequence_time += timing_ns;
				
				struct debug_data_t *buf = k_malloc(sizeof(struct debug_data_t));
				if (buf != NULL) {
					buf->time = timing_ns;
					k_fifo_put(&data_fifo, buf);
				}
				k_yield();

			}
            else if (color == 'T' || color == 't') {
                if (Tlaskuri == 0) {
                    i = 0;
                    Tlaskuri++;
                } else {
                    i++;
                }
                k_msleep(1000);
            } else {
                i++;
            }
        }
       // printk("yhteenlaskettu kokonaisaika: %lld\n", total_sequence_time);
    }
}
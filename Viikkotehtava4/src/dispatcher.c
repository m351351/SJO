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
#include "led_task.h"
#include <assert.h>
#include <sys/types.h>

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;

void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
    while (true) {
        // Receive dispatcher data from uart_task fifo
        struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
        char sequence[20];
        memset(sequence,0,sizeof(sequence));
		strncpy(sequence, rec_item->msg, sizeof(sequence)-1);
		k_free(rec_item);

        // paina pelkkää enteriä niin tämä iskee päälle:
        __ASSERT(strlen(sequence) > 0, "Tyhjä merkkijono havaittu dispatcherissa!");

        uint64_t total_sequence_time = 0;
        int Tlaskuri = 0;
        int i = 0;

        timing_start();
        timing_t seq_start = timing_counter_get();

        while (sequence[i] != '\0')
		{
			char color = sequence[i];
			i++;

            

			if (color == 'T' || color == 't')
			{
				if (Tlaskuri == 0)
                {
					i = 0;
					printk("UUdestaan");
					Tlaskuri = 1;
					continue;
				}   
                else
                {
				    continue;
			    }
            }
            
            else if (color == 'D' || color == 'd')
            {
                debug_enabled = !debug_enabled;
                printk("Debug-tulostukset: %s\n", debug_enabled ? "PAALLA" : "POIS");
                continue;
            }
                        

			int duration = 1000;
			
            if (sequence[i]==',')
			{
				i++;
				duration=atoi(&sequence[i]);
                if (duration < 0) {
                    printk("VIRHE 2: Syötetty kesto on negatiivinen (%d)!\n", duration);
                 }
                 // syötä negatiivinen arvo niin tämä laukeaa
                __ASSERT(duration >= 0, "Kesto ei saa olla negatiivinen!");

				while (sequence[i] >= '0' && sequence[i] <= '9')
				{
					i++;
				}
			}

			k_mutex_lock(&val_mutex, K_FOREVER);
			active_color = color;
			active_duration = duration;

            timing_t start_time = timing_counter_get();

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
            else
            {
                printk("VIRHE 3: Tuntematon merkki (%c) vastaanotettu!\n", color);
                // syötä joku muu kuin r ,g, y, t tai d niin tämä laukeaa
                __ASSERT(0, "Tuntematon merkki sekvenssissä!");
            }


			k_mutex_unlock(&val_mutex);
			k_sem_take(&release_sem, K_FOREVER);

            timing_t end_time = timing_counter_get();
            uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
            total_sequence_time += timing_ns;

            struct debug_data_t *buf = k_malloc(sizeof(struct debug_data_t));
            if (buf != NULL)
            {
                buf->time = timing_ns;
                k_fifo_put(&data_fifo, buf);
            }
            k_yield();
		}

		timing_t seq_end = timing_counter_get();
        timing_stop();
        uint64_t total_ns = timing_cycles_to_ns(timing_cycles_get(&seq_start, &seq_end));
        printk("Sekvenssin yhteenlaskettu kokonaisaika: %llu ns\n", total_ns);
	}
}

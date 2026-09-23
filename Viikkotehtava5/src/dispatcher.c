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
#include <ctype.h>

extern const struct gpio_dt_spec red;
extern const struct gpio_dt_spec green;

#define COMMAND_OK 0
//definaa tänne ne samat virhekoodit mitä oli TimeParser.h:ssa
#define TIME_LEN_ERROR      -1 // palautetaan jos merkkejä on erimäärä kuin 6
#define TIME_ARRAY_ERROR    -2 // jos palautusarvo on tyhjä (NULL) tai sisältää vääriä merkkejä
#define TIME_VALUE_ERROR    -3 // palautetaan jos sekunteja tai minuutteja yli 59 tai tunteja yli 23
#define EASTEREGG           -4 // palauttaa tämän jos antaa arvoksi mun syntymäkellonajan

extern struct k_timer timer;
extern void timer_handler(struct k_timer *timer_id);

int time_parse(char *time) {

	// how many seconds, default returns error
	//int seconds = TIME_LEN_ERROR;

	// TODO: Check that string is not null
	if(time == NULL){
		return TIME_ARRAY_ERROR;
	}

	if(strlen(time) != 6){
		return TIME_LEN_ERROR;
	}

	for (int i = 0; i < 6; i++){
		if(!isdigit(time[i])){
			return TIME_ARRAY_ERROR;
		}
	}

	// HHMMSS
	// HH values [0]
	// MM values [1]
	// SS values [2] 
		// Parse values from time string
	// For example: 124033 -> 12hour 40min 33sec
    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours
	// Now you have:
	// values[0] hour
	// values[1] minute
	// values[2] second

	// TODO: Add boundary check time values: below zero or above limit not allowed
	// limits are 59 for minutes, 23 for hours, etc


	// TODO: Calculate return value from the parsed minutes and seconds
	// Otherwise error will be returned!
	// seconds = ...
	if (values[0] < 0 || values[0] > 23 ||
        values[1] < 0 || values[1] > 59 ||
        values[2] < 0 || values[2] > 59) {
        return TIME_VALUE_ERROR;
    }

	if (values[0] == 17 &&
        values[1] == 44 &&
        values[2] == 00) {
        return EASTEREGG;
    }

	int seconds = (values[0] * 3600) + (values[1] * 60) + values[2];
	
	if (seconds == 0){
		return TIME_VALUE_ERROR;
	}

	return seconds;
}


void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
    
    while (true) {
        // Receive dispatcher data from uart_task fifo
        struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
        char sequence[20];
        memset(sequence,0,sizeof(sequence));
		strncpy(sequence, rec_item->msg, sizeof(sequence)-1);
		k_free(rec_item);

        bool just_numbers = true;
        if(strlen(sequence)==6)
        {
            for(int j = 0; j<6; j++)
            {
                if(!isdigit(sequence[j]))
                {
                    just_numbers = false;
                    break;
                }
             }
        } 
        else
        {
            just_numbers = false;
        }
        
       

        if (just_numbers){
        int ret = time_parse(sequence);
        // check parser return value
        if (ret > COMMAND_OK) {
            // send signal / message to mailbox
            // tänne keskeytystimer teemun ohjeista debug: k_time_init
            // Timer initialization
            printk("hyvä aika, ajetaan ajastin \n");
            k_timer_init(&timer, timer_handler, NULL);
            k_timer_start(&timer, K_SECONDS(ret), K_FOREVER);
            continue;
        } 
        else if (ret == TIME_VALUE_ERROR){
            printk("Sekunteja tai minuutteja yli 59 tai tunteja yli 23");
            continue;
        }
        else if (ret == TIME_ARRAY_ERROR){
            printk("Palautusarvo on NULL, ei siällä mitään tai sisältää vääriä merkkejä");
            continue;
        }
        else if (ret == TIME_LEN_ERROR){
            printk("Merkkejä on erimäärä kuin 6");
            continue;
        }
        else if (ret == EASTEREGG){
            printk("Annettu aika on Merin syntymäaika");
            k_timer_init(&timer, timer_handler, NULL);
            k_timer_start(&timer, K_SECONDS(ret), K_NO_WAIT);
            continue;
            
        }
    }




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



//tämä on vika vaihe vk5 tehtävässä. Voi laittaa hommaksi mitä vaan
// control led using timer interrupt
void timer_handler(struct k_timer *timer_id) {
    extern int led_state;
    extern const struct gpio_dt_spec blue;
	if (led_state == false) {
		gpio_pin_set_dt(&blue,1);
		led_state = true;
	} else {
		gpio_pin_set_dt(&blue,0);
		led_state = false;
	}
}

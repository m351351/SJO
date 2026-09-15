// Meri-Tuulia Turtinen
// TVT24SPL

// Tavoite kolme pistettä
// laitettu buttonit omiin moduuleihin. Järkyttävä taistelu asian kanssa että sain ne toimimaan
// Tässä nyt kahden pisteen edestä toimintaa.

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "button.h"



static const struct  gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static const struct gpio_dt_spec yellow = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

#define STACKSIZE 500
#define PRIORITY 5
//void red_led_task(void *, void *, void*);
//void green_led_task(void *, void *, void*);
//void blue_led_task(void *, void *, void*);
//void yellow_led_task(void *, void *, void*);
void led_task(void *, void *, void*);

K_THREAD_DEFINE(red_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(green_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(blue_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(yellow_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);

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

	ret = gpio_pin_configure_dt(&blue, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) return ret;
	gpio_pin_set_dt(&blue, 0);

	ret = gpio_pin_configure_dt(&yellow, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) return ret;
	gpio_pin_set_dt(&yellow, 0);

	printk("Led initialized ok\n");
	return 0;
}

int main(void)
{
	init_led(); 
	init_button();

	while (1) {
		k_msleep(100);
	}

	return 0;
}

void paussin_tsekkaus(int total_ms){
        int kulunut_aika = 0;
        while (kulunut_aika < total_ms) {
            if (tilakone == 4) {
                k_msleep(100); // Sleep for a short time to avoid busy waiting
            } else {
                k_msleep(100); // Sleep for a short time to avoid busy waiting
                kulunut_aika += 100; // Increment elapsed time
            }
        }
}


void led_task(void *, void *, void*) {
	printk("pyöritelhäs\n");

	while (true) {

		if (tilakone == 4){
                     k_sleep(K_SECONDS(1));
                     continue; // Skip the rest of the loop if tilakone is 0           
                }

                switch (tilakone) {
                    case 0:
                        gpio_pin_set_dt(&red, 1);
                        printk("Red on\n");
                        paussin_tsekkaus(1000); 
                        
                        gpio_pin_set_dt(&red, 0);
                        printk("Red off\n");
                        paussin_tsekkaus(1000); 
                        if (tilakone != 4) tilakone = 1; // Move to the next state only if tilakone is not 0
                        break;

                    case 1:
                        gpio_pin_set_dt(&red, 1);
			gpio_pin_set_dt(&green, 1);;
                        printk("Yellow on\n");
                        paussin_tsekkaus(1000); 
                        gpio_pin_set_dt(&red, 0);
			gpio_pin_set_dt(&green, 0);
                        printk("Yellow off\n");
                        paussin_tsekkaus(1000);
                        if (tilakone != 4) tilakone = 2; // Move to the next state only if tilakone is not 4
                        break;

                    case 2:
                        gpio_pin_set_dt(&green, 1);
                        printk("Green on\n");
                        paussin_tsekkaus(1000); 
                        gpio_pin_set_dt(&green, 0);
                        printk("Green off\n");
                        paussin_tsekkaus(1000); 
                        if (tilakone != 4) tilakone = 0; // Move to the next state only if tilakone is not 4
                        break;

                    default:
                        k_sleep(K_SECONDS(1));
                        break;
                }
        }
                 
}


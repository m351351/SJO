'''
Tavoite oli kolme pistettä, mutta en saanut 2 pisteen asiaa toimimaan
Yritin tehdä koodista modulaarisemman ja helpostiluettavan laittamalla button.c ja button.h tiedostot
Ohjelma meni ihan nätisti tuon liikennevaloasian osalta, mutta mitään muuta kontaktia
en sitten laitteeseen enää saanutkaan. Luennossa jossa opettaja kävi läpi 
miten harjoitus tulee tehdä ei ollut ääntä, enkä löytänyt ohjevideota jossa olisi ollut äänikin mukana.

Teoriassa tiedän miten ohjelman pitäisi toimia, mutta en kertakaikkiaan saanut sitä toimimaan käytännössä. Käytin tähän kokonaisen työpäivän.

Perehdyn aiheeseen lisää myöhemmin, josko luennoilla selviäisi asioita tai saisin opiskelutovereilta vertaistukea.

'''

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "button.h"

static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static const struct gpio_dt_spec yellow = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

#define STACKSIZE 500
#define PRIORITY 5
void red_led_task(void *, void *, void*);
void green_led_task(void *, void *, void*);
void blue_led_task(void *, void *, void*);
void yellow_led_task(void *, void *, void*);

K_THREAD_DEFINE(red_thread,STACKSIZE,red_led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(green_thread,STACKSIZE,green_led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(yellow_thread,STACKSIZE,yellow_led_task,NULL,NULL,NULL,PRIORITY,0,0);

int tilakone = 0; 

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

void red_led_task(void *, void *, void*) {
	printk("Red led thread started\n");
	while (true) {
		if (tilakone == 0){
			gpio_pin_set_dt(&red, 1);
			printk("Red on\n");
			k_sleep(K_SECONDS(1));
			gpio_pin_set_dt(&red, 0);
			printk("Red off\n");
			k_sleep(K_SECONDS(1));
			tilakone = 1;
		}
		k_sleep(K_SECONDS(1));
	}
}

void yellow_led_task(void *, void *, void*) {
	printk("Yellow led thread started\n");
	while (true) {
		if (tilakone == 1){
			gpio_pin_set_dt(&red, 1);
			gpio_pin_set_dt(&green, 1);
			printk("Yellow on\n");
			k_sleep(K_SECONDS(1));
			gpio_pin_set_dt(&red, 0);
			gpio_pin_set_dt(&green, 0);
			printk("Yellow off\n");
			tilakone = 2;
		}
		k_sleep(K_SECONDS(1));
	}
}

void green_led_task(void *, void *, void*) {
	printk("Green led thread started\n");
	while (true) {
		if (tilakone == 2){
			gpio_pin_set_dt(&green, 1);
			printk("Green on\n");
			k_sleep(K_SECONDS(1));
			gpio_pin_set_dt(&green, 0);
			printk("Green off\n");
			k_sleep(K_SECONDS(1));
			tilakone = 0;
		}
		k_sleep(K_SECONDS(1));
	}
}
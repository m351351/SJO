#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include "led_task.h"

const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);


struct k_mutex val_mutex;
struct k_condvar red_cv;
struct k_condvar green_cv;
struct k_condvar yellow_cv;
struct k_sem release_sem;

char active_color = '\0';
int active_duration = 1000;

int init_led() {
	int ret;

	ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
	if (ret < 0){
		return ret;
	}
	
	ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
	if (ret < 0){ 
        return ret;
    }

	gpio_pin_set_dt(&red, 0);
    gpio_pin_set_dt(&green, 0);
	printk("Led initialized ok\n");
	return 0;
}

void red_led_task(void *unused1, void *unused2, void *unused3)
{
    while (true){
        k_mutex_lock(&val_mutex, K_FOREVER);
        k_condvar_wait(&red_cv, &val_mutex, K_FOREVER);

        int duration = active_duration;
        k_mutex_unlock(&val_mutex);

        gpio_pin_set_dt(&red, 1);
        k_msleep(duration);
        gpio_pin_set_dt(&red, 0);

        k_sem_give(&release_sem);
    }
}

void yellow_led_task(void *unused1, void *unused2, void *unused3)
{
    while (true){
        k_mutex_lock(&val_mutex, K_FOREVER);
        k_condvar_wait(&yellow_cv, &val_mutex, K_FOREVER);

        int duration = active_duration;
        k_mutex_unlock(&val_mutex);

        gpio_pin_set_dt(&red, 1);
        gpio_pin_set_dt(&green, 1);
        k_msleep(duration);
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);

        k_sem_give(&release_sem);
    }
}

void green_led_task(void *unused1, void *unused2, void *unused3)
{
    while (true){
        k_mutex_lock(&val_mutex, K_FOREVER);
        k_condvar_wait(&green_cv, &val_mutex, K_FOREVER);

        int duration = active_duration;
        k_mutex_unlock(&val_mutex);

        gpio_pin_set_dt(&green, 1);
        k_msleep(duration);
        gpio_pin_set_dt(&green, 0);

        k_sem_give(&release_sem);
    }
}
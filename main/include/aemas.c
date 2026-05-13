
#include "aemas.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
void gpio_init(void) {
	gpio_reset_pin(RELAY_1);
	gpio_reset_pin(RELAY_2);
	gpio_reset_pin(RELAY_3);
	gpio_reset_pin(RELAY_4);

	gpio_set_direction(RELAY_1, GPIO_MODE_OUTPUT);
	gpio_set_direction(RELAY_2, GPIO_MODE_OUTPUT);
	gpio_set_direction(RELAY_3, GPIO_MODE_OUTPUT);
	gpio_set_direction(RELAY_4, GPIO_MODE_OUTPUT);
}

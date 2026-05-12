
#include <stdio.h>
#include "include/led.h"
#include "esp_err.h"
#include "led_strip_types.h"
#include "esp_log.h"
static const char *TAGL = "LED";
void configure_led(void)
{
	led_strip_config_t strip_conf = {
		.strip_gpio_num = LED,
		.max_leds = 1,
	};
	led_strip_rmt_config_t rmt_conf = {
		.resolution_hz = 10 * 1000 * 1000,
		.flags.with_dma = false,
	};
	ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_conf, &rmt_conf, &led_strip));
	ESP_LOGI(TAGL, "LED configured...");
}

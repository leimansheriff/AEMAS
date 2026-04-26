
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "led_strip.h"
#include "mk3.h"
#include "soc/gpio_num.h"
#include "usbconf.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "led.h"
#include "aemas.h"

static const char *TAG = "STATE";
static const char *TAGB = "BUTTON";
led_strip_handle_t led_strip = NULL;
void app_main(void)
{
	gpio_reset_pin(BUTTON);
	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

	gpio_reset_pin(RELAY_1);
	gpio_reset_pin(RELAY_2);
	gpio_reset_pin(RELAY_3);
	gpio_reset_pin(RELAY_4);

	gpio_set_direction(RELAY_1, GPIO_MODE_INPUT);
	gpio_set_direction(RELAY_2, GPIO_MODE_INPUT);
	gpio_set_direction(RELAY_3, GPIO_MODE_INPUT);
	gpio_set_direction(RELAY_4, GPIO_MODE_INPUT);
	sys_info_t info;
	usb_drivers_install();
	configure_led();

	aems_state_t current_state = INIT;
	while (1) {
		int button_state = gpio_get_level(BUTTON);
		switch (current_state) {
			case INIT:
				led_strip_set_pixel(led_strip, 0, 255, 255, 255);
				led_strip_refresh(led_strip);
				esp_err_t condition = usbconf(&info);
				if (condition == ESP_OK) {
					ESP_LOGI(TAG, " %s mode: USB initialization successful", current_state);
					current_state = NORMAL;
				}
				else {
					ESP_LOGE(TAG, "USB initialization failed... trying again");
					vTaskDelay(pdMS_TO_TICKS(2000));
				}
				if (button_state == 0)
				{
					ESP_LOGI(TAGB, "BUTTON pressed switching to user USER_OVERIDE mode");
					current_state = USER_OVERIDE;
				}
			break;
			case NORMAL:
				if (info.voltage < 52) {
					current_state = SHEDDING;
				}
			break;
			case SHEDDING:
			break;
			case USER_OVERIDE:
				led_strip_set_pixel(led_strip, 0, 255, 255, 255255);
				led_strip_refresh(led_strip);
			break;
		}
	}
}

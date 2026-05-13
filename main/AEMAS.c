
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <string.h>
#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
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
#include "wifi.h"
#include "server.h"

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

void load_init(void) {
	gpio_set_level(RELAY_1, ON);
	ESP_LOGI("LOAD", "LOAD 1: ON");
	vTaskDelay(pdMS_TO_TICKS(200));
	gpio_set_level(RELAY_2, ON);
	ESP_LOGI("LOAD", "LOAD 2: ON");
	vTaskDelay(pdMS_TO_TICKS(200));
	gpio_set_level(RELAY_3, ON);
	ESP_LOGI("LOAD", "LOAD 3: ON");
	vTaskDelay(pdMS_TO_TICKS(200));
	gpio_set_level(RELAY_4, ON);
	ESP_LOGI("LOAD", "LOAD 4: ON");
	vTaskDelay(pdMS_TO_TICKS(200));
}

void load_state(int load, int state) {
	switch (state) {
		case ON:
			gpio_set_level(load, state);
		break;
		case OFF:
			gpio_set_level(load, state);
		break;
		default:
			ESP_LOGI("LOAD", "Invalid state");
		break;
	}
}

static const char *TAG = "STATE";
void app_main(void)
{
	sys_info_t info = {0};
	sys_info_t threshold_voltage;
	start_wifi();
	start_webserver(&info, &threshold_voltage);
	usb_drivers_install();
	gpio_init();
	aems_state_t current_state = INIT;
	while (1) {
		switch (current_state) {
			case INIT:
				esp_err_t condition = usbconf(&info);
				if (condition == ESP_OK) {
					load_init();
					current_state = NORMAL;
					ESP_LOGI(TAG, "Switching state to normal");
				}
				else {
					ESP_LOGE(TAG, "USB initialization failed... trying again");
					vTaskDelay(pdMS_TO_TICKS(2000));
				}
			break;
			case NORMAL:
				send_request();
				ESP_LOGI(TAG, "Battery: %f\n Current: %f Power: %f", info.voltage, info.current, info.power_out);
				if (info.voltage < threshold_voltage.voltage) {
					current_state = SHEDDING;
					ESP_LOGI(TAG, "Switching state to shedding");
				}
				vTaskDelay(pdMS_TO_TICKS(5000));
			break;
			case SHEDDING:
				vTaskDelay(pdMS_TO_TICKS(300));
				load_state(RELAY_3, OFF);
				load_state(RELAY_4, OFF);
				if (info.voltage > threshold_voltage.voltage)
				{
					vTaskDelay(pdMS_TO_TICKS(200));
					current_state = NORMAL;
				}
			break;
			case USER_OVERIDE:
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

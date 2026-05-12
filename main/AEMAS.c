
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <string.h>
#include "esp_err.h"
#include "esp_http_server.h"
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
#include "wifi.h"
#include "server.h"

static const char *TAG = "STATE";
void app_main(void)
{
	sys_info_t info;
	sys_info_t threshold_voltage;
	start_wifi();
	start_webserver(&info, &threshold_voltage);
	usb_drivers_install();
	aems_state_t current_state = INIT;
	while (1) {
		switch (current_state) {
			case INIT:
				esp_err_t condition = usbconf(&info);
				if (condition == ESP_OK) {
					ESP_LOGI(TAG, "Battery: %f\n Current: %f Soc: %f, power_out: %f", info.voltage, info.current, info.soc, info.power_out);
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
				ESP_LOGI(TAG, "Battery: %f\n Current: %f", info.voltage, info.current);
				vTaskDelay(pdMS_TO_TICKS(5000));
			break;
			case SHEDDING:
			break;
			case USER_OVERIDE:
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

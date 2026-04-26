
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_log.h"
#include "include/mk3.h"

static const char *TAG = "MK3";

void victron_parser(const uint8_t *buffer, size_t data_len, void *arg) 
{
	sys_info_t *sys_info = (sys_info_t *)arg; // why not use this casting in the function directly 
	uint8_t checksum = 0;
	if (data_len < 12) {
		return;
	}
	switch (buffer[1]) {
		case 0x20: 
			for (int i = 0; i < data_len; i++)
			{
				checksum += buffer[i];
			}
			if (checksum == 0)
			{
				ESP_LOGI(TAG, "Checksum valid");
				int raw_batv = (buffer[7] << 8) | buffer[8];
				int raw_current = (buffer[9] << 16) | (buffer[10] << 8) | buffer[11];
				sys_info->voltage = raw_batv * 0.1f;
				sys_info->current = raw_current *0.01f;
			}
			else {
				ESP_LOGE(TAG, "Checksum Invalid!! copying again");
			}
		break;
		case 0x57:
			for (int i = 0; i < data_len; i++)
			{
				checksum += buffer[i];
			}
			if (checksum == 0)
			{
				ESP_LOGI(TAG, "Checksum valid");
			}
			else {
				ESP_LOGE(TAG, "Checksum Invalid!! copying again");
			}
		break;
		default:
			ESP_LOGW(TAG, "Invalid reply header 0x%X", buffer[1]);
		break;
	}
}

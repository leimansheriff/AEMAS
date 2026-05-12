
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_log.h"
#include "include/mk3.h"

static const char *TAG = "MK3";
extern int current_idx;
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
		case 0xFF:
			for (int i = 0; i < data_len; i++)
			{
				checksum += buffer[i];
			}
			if (checksum == 0)
			{
				ESP_LOGI(TAG, "Checksum valid");
				if (buffer[2] == 0x57 && buffer[3] == 0x85) {
					int16_t raw_data = (buffer[5] << 8) | buffer[4];
					if (current_idx == 1) {
						sys_info->soc = raw_data;
						ESP_LOGI(TAG, "SoC updated: %d\%", sys_info->soc);
					}
					else if (current_idx == 2) {
						sys_info->power_out = raw_data;
						ESP_LOGI(TAG, "Out_Power updated: %d,", sys_info->power_out);
					}
				}
				else {
					ESP_LOGE(TAG, "Victron Returned error: 0x%X", buffer[3]);
				}
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

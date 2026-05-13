
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_log_buffer.h"
#include "include/mk3.h"

static const char *TAG = "MK3";
extern int current_idx;
void victron_parser(const uint8_t *buffer, size_t data_len, void *arg) 
{
	sys_info_t *sys_info = (sys_info_t *)arg;
    static uint8_t rx_bucket[256];
    static uint16_t bucket_index = 0;

    for (int i = 0; i < data_len; i++) {
        if (bucket_index < sizeof(rx_bucket)) {
            rx_bucket[bucket_index] = buffer[i];
            bucket_index++;
        } else {
            bucket_index = 0; 
        }
    }

    if (bucket_index > 0) {
        uint8_t expected_total_len = rx_bucket[0] + 2;

        if (bucket_index >= expected_total_len) {
            uint8_t checksum = 0;
            for (int i = 0; i < expected_total_len; i++) checksum += rx_bucket[i];

            if (checksum == 0) {
                switch (rx_bucket[1]) {
                    case 0x20:
                        
                        // 1. IS IT DC DATA?
                        if (rx_bucket[2] == 0xFD) { 
                            uint16_t raw_batv = (rx_bucket[8] << 8) | rx_bucket[7];
                            sys_info->voltage = raw_batv * 0.01f; 

                            int32_t raw_current = (rx_bucket[11] << 16) | (rx_bucket[10] << 8) | rx_bucket[9];
                            if (raw_current & 0x800000) raw_current |= 0xFF000000;
                            sys_info->current = raw_current * 0.1f; 
                        } 
                        
                        // 2. IS IT AC DATA?
                        else if (rx_bucket[2] == 0x01) {
                            // Extract AC Voltage (Bytes 11 & 12, Scale 0.01)
                            uint16_t raw_ac_v = (rx_bucket[12] << 8) | rx_bucket[11];
                            float ac_voltage = raw_ac_v * 0.01f;

                            // Extract AC Current (Bytes 13 & 14, Scale 0.1)
                            int16_t raw_ac_i = (rx_bucket[14] << 8) | rx_bucket[13];
                            float ac_current = raw_ac_i * 0.1f;

                            // Power = Volts * Amps (Using absolute value so UI shows a positive draw)
                            int raw_power = (int)(ac_voltage * ac_current);
                            if (raw_power < 0) raw_power = -raw_power; 
                            
                            sys_info->power_out = raw_power;
                        }
                        break;
                        
                    default:
                        // Silently ignore Version heartbeats and other noise
                        break;
                }
            } else {
                ESP_LOGE("PARSER", "Checksum failed on assembled frame.");
            }
            bucket_index = 0; 
        }
    }
}

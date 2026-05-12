
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "mk3.h"
#include "esp_log.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "usb/cdc_host_types.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "usb/usb_types_cdc.h"
#include "usb/usb_types_stack.h"
#include "include/usbconf.h"
#include "usb/vcp_ftdi.h"

static const char *TAG = "USB";
cdc_acm_dev_hdl_t cdc_dev_hdl = NULL;
int current_idx = -1;
msg_rqst_t aems_request[] = {
	{.command = 0x70, .data = {0x00}, .data_len = 1, .label = "DC_INFO"},
	{.command = 0x57, .data = {0x30, 0x0D, 0x00}, .data_len = 3, .label = "SOC"},
	{.command = 0x57, .data = {0x30, 0x10, 0x00}, .data_len = 3, .label = "power_out"}
};

void victron_request(msg_rqst_t *data, uint8_t *frame)
{
	uint8_t checksum = 0;
	frame[0] = data->data_len + 2; // Length
	frame[1] = 0xFF; // Header
	frame[2] = data->command; // Command
	for (int i = 0; i < data->data_len; i++)
	{
		frame[3 + i] = data->data[i];
	}
	uint8_t total_size = frame[0] + 1;
	for (int i = 0; i < total_size; i++)
	{
		checksum += frame[i];
	}
	frame[total_size] = -checksum;
	ESP_LOGI(TAG, "Sending request for %s", data->label);
}

void send_request(void)
{
	uint8_t tx_buffer[32];
	for (int i = 0; i < 3; i++)
	{
		current_idx = i;
		victron_request(&aems_request[i], tx_buffer); // why does aems_request[] have & and tx_buffer does not ? Becuase is an array of srtucts and we need access to particular elements of the array not the entire array 
		size_t total_len = tx_buffer[0] + 2;
		esp_err_t error = cdc_acm_host_data_tx_blocking(cdc_dev_hdl, tx_buffer, total_len, 1000);
		if (error != ESP_OK) {
			ESP_LOGE(TAG, "CDC device failed");
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	current_idx = -1;
}

static bool handle_rx(const uint8_t *data, size_t data_len, void *arg)
{
	victron_parser(data, data_len, arg);
	ESP_LOGI(TAG, "Victron message received");
	return true;
}

static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
	switch (event->type) {
		case CDC_ACM_HOST_ERROR:
			ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %i", event->data.error);
			break;
		case CDC_ACM_HOST_DEVICE_DISCONNECTED:
			ESP_LOGI(TAG, "Device suddenly disconnected");
			ESP_ERROR_CHECK(cdc_acm_host_close(event->data.cdc_hdl));
			break;
		case CDC_ACM_HOST_SERIAL_STATE:
			ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
			break;
		default:
			ESP_LOGW(TAG, "Unsupported CDC event: %d (possibly suspend/resume)", event->type);
			break;
	}
}
static void usb_lib_task(void *arg) {
	while (1) {
		uint32_t event_flags;
		usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
		if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
			ESP_ERROR_CHECK(usb_host_device_free_all());
		}
	}
}
esp_err_t usbconf(sys_info_t *info)
{
	const cdc_acm_host_device_config_t dev_config = {
		.connection_timeout_ms = 1000,
		.out_buffer_size = 512,
		.in_buffer_size = 512,
		.user_arg = info,
		.event_cb = handle_event,
		.data_cb = handle_rx
	};
	ESP_LOGI(TAG, "Opening CDC-ACM Device 0x%04X:0x%04X", VID, PID);
	esp_err_t err = ftdi_vcp_open(0x6015, 0, &dev_config, &cdc_dev_hdl);
	if (err == ESP_OK && cdc_dev_hdl != NULL) {
		ESP_LOGI(TAG, "MK3 connected MK3 ID: 0x%04X:0x%04X", VID, PID);
		cdc_acm_line_coding_t line_coding = {
			.dwDTERate = 2400,
			.bCharFormat = 0,
			.bParityType = 0,
			.bDataBits = 8
		};
		ESP_ERROR_CHECK(cdc_acm_host_line_coding_set(cdc_dev_hdl, &line_coding));
		vTaskDelay(pdMS_TO_TICKS(200));
	}
	else {
		ESP_LOGE(TAG, "Failed to open device. Error: %s", esp_err_to_name(err));
	}
	return err;
}
void usb_drivers_install(void){
	ESP_LOGI(TAG, "Installing USB Host");
	const usb_host_config_t host_config = {
		.skip_phy_setup = false, 
		.intr_flags = ESP_INTR_FLAG_LOWMED,
	};
	//what is the use of using ESP_ERROR_CHECK()
	ESP_ERROR_CHECK(usb_host_install(&host_config));
	xTaskCreate(usb_lib_task, "usb events", 4096, NULL, 10, NULL);
	ESP_LOGI(TAG, "Installing CDC-ACM driver");

	ESP_ERROR_CHECK(cdc_acm_host_install(NULL));
	vTaskDelay(pdMS_TO_TICKS(200));
}


#ifndef USBCONF_H
#define USBCONF_H
#include <stdint.h>
#include "esp_err.h"
#include "mk3.h"
#include "usb/cdc_host_types.h"
#define VID (0x0403)
#define PID (0x6015)

typedef struct {
	uint8_t command;
	uint8_t data[3];
	uint8_t data_len;
	const char *label;
} msg_rqst_t;

esp_err_t usbconf(sys_info_t *info);
void usb_drivers_install(void);
void victron_request(msg_rqst_t *data, uint8_t *frame);
void send_request(void);

#endif // !USBCONF_H


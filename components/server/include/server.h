#ifndef SERVER_H
#define SERVER_H
#include "esp_http_server.h"

#define HTTP_QUERY_KEY_MAX_LEN (64)
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");
httpd_handle_t start_webserver(void *info, void *threshold_voltage);
#endif // !SERVER_H
#define SERVER_H


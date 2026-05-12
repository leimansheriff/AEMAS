#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "http_parser.h"
#include "server.h"
#include "mk3.h"
#include "cJSON.h"
#include <sys/param.h>

static const char *TAG = "SERVER";

esp_err_t root_get_handler(httpd_req_t *req) {
	const size_t file_size = (index_html_end - index_html_start);
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, file_size);
	return ESP_OK;
}
httpd_uri_t root_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
	.user_ctx = NULL,
};

esp_err_t status_get_handler(httpd_req_t *req) {
	sys_info_t *victron = (sys_info_t *) req->user_ctx;
	char json_response[100];
	snprintf(json_response, sizeof(json_response), "{\"voltage\": %.1f, \"current\": %.1f }", victron->voltage, victron->current);
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}
httpd_uri_t status_uri = {
	.uri = "/api/status",
	.method = HTTP_GET, 
	.handler = status_get_handler,
	.user_ctx = NULL,
};

esp_err_t status_post_handler(httpd_req_t *req) {
	sys_info_t *vic_get = (sys_info_t *) req->user_ctx;
	char buf[100];
	int ret, remaining = req->content_len;

	if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
		return ESP_FAIL;
	}
	buf[ret] = '\0';
	cJSON *root = cJSON_Parse(buf);
	if (root != NULL) { 
		cJSON *shed_val = cJSON_GetObjectItem(root, "shed_voltage");

		if (shed_val != NULL) {
			vic_get->voltage = shed_val->valuedouble;
			ESP_LOGI(TAG, "New shedding threshold: %f", vic_get->voltage);
		}
		cJSON_Delete(root);
}
	httpd_resp_send(req, "Success", HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}
httpd_uri_t button_uri = {
	.uri = "/api/config", 
	.method = HTTP_POST, 
	.handler = status_post_handler,
	.user_ctx = NULL,
};

httpd_handle_t start_webserver(void *info, void *threshold_voltage) {
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	ESP_LOGI(TAG, "starting server on port: '%d'", config.server_port);

	if (httpd_start(&server, &config) == ESP_OK) {
		status_uri.user_ctx = info;
		button_uri.user_ctx = threshold_voltage;
		ESP_LOGI(TAG, "Registering URI handlers");
		httpd_register_uri_handler(server, &root_uri);
		httpd_register_uri_handler(server, &status_uri);
		httpd_register_uri_handler(server, &button_uri);
		return server;
	}
	ESP_LOGI(TAG, "Error starting server");
	return NULL;
}


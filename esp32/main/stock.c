#include <freertos/FreeRTOS.h>

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>
#include <esp_tls.h>
#include <esp_http_client.h>
#include <sys/param.h>

#include "stock.h"

#define PRICE_LEN 15

static char* TAG = "stock";
static esp_http_client_handle_t http_client;

static esp_err_t http_evt_handler(esp_http_client_event_t *evt)
{
	static int read_len;

	switch(evt->event_id) {
		case HTTP_EVENT_ON_HEADER:
			ESP_LOGD(TAG,
				"HTTP_EVENT_ON_HEADER, key=%s, value=%s",
				evt->header_key,
				evt->header_value);
			break;
		case HTTP_EVENT_ON_DATA:
			int copy_len = 0;
			int cont_len = esp_http_client_get_content_length(evt->client);
			char **user_data = (char **) evt->user_data;

			if (*user_data == NULL) {
				*user_data = calloc(cont_len + 1, sizeof(char));
				if (!evt->user_data)
					ESP_LOGE(TAG, "calloc() failed for response buffer");
			}

			if (evt->user_data) {
				copy_len = MIN(evt->data_len, (cont_len - read_len));
				if (copy_len) {
					memcpy((*(char **) evt->user_data) + read_len,
						evt->data, copy_len);
					read_len += copy_len;
				}

				ESP_LOGD(TAG, "HTTP response size = %d bytes", read_len);
			}
			break;
		case HTTP_EVENT_ON_FINISH:
			read_len = 0;

			ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
			ESP_LOGD(TAG, "%s", *(char **) evt->user_data);
			ESP_LOGD(TAG,
				"response status=%d",
				esp_http_client_get_status_code(evt->client));

			break;
		case HTTP_EVENT_DISCONNECTED:
			int mbedtls_err = 0;
			esp_err_t err;

			ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

			err = esp_tls_get_and_clear_last_error(
				(esp_tls_error_handle_t) evt->data,
				&mbedtls_err,
				NULL);

			if (err != 0) {
				ESP_LOGE(TAG, "Last esp error code: 0x%x", err);
				ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
			}

			read_len = 0;
			break;
		default:
			break;
	}

	return ESP_OK;
}

static inline int min(int a, int b) {
	return a < b ? a : b;
}

static inline int max(int a, int b) {
	return a > b ? a : b;
}

static inline int get_price_value(char **s, char *buf, int n) {
	int i;

	for (i = 0; **s && **s != '\n'; (*s)++) {
		if (i < n - 1)
			buf[i++] = **s;
	}

	buf[i] = '\0';
	return (int)(atof(buf) * 100);
}

static inline void parse(char *s, struct stock_data *sd)
{
	int i;
	const int n = 10;

	char buf_t[n];
	char buf_p[n];

	sd->price_max = 0;
	sd->price_min = INT_MAX;

	for (i = 0; *s && *s != '\n'; s++) {
		if (i < n - 1)
			buf_t[i++] = *s;
	}

	buf_t[i] = '\0';

	if (*s)
		s++;

	sd->price_ref = get_price_value(&s, buf_p, n);

	if (*s)
		s++;

	for (i = 0; *s && *s != '\n' && i < sd->prices_maxlen; i++) {
		sd->prices[i] = get_price_value(&s, buf_p, n);
		sd->price_min = min(sd->prices[i], sd->price_min);
		sd->price_max = max(sd->prices[i], sd->price_max);
	}

	sd->prices_len = i;

	if (!sd->price_ref)
		sd->price_ref = sd->price_max + 1;

	snprintf(sd->ticker, TICKER_LEN, "%s: %s", buf_t, buf_p);
}

void stock_get_data(struct stock_data *sd)
{
	esp_err_t rc;
	char *buf = NULL;

	esp_http_client_set_user_data(http_client, &buf);	

	for(;;) {
		rc = esp_http_client_perform(http_client);
		if (rc != ESP_ERR_HTTP_EAGAIN)
			break;

		vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
	}

	parse(buf, sd);
	free(buf);
}

void stock_init(void)
{
	esp_http_client_config_t conf = {
		.url = CONFIG_STOCK_API_URL,
		.is_async = true,
		.timeout_ms = 5000,
		.event_handler = http_evt_handler,
		.disable_auto_redirect = true,
		.auth_type = HTTP_AUTH_TYPE_BASIC,
		.max_authorization_retries = -1
	};

	http_client = esp_http_client_init(&conf);
}

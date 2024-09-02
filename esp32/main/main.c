#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <time.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <nvs_flash.h>

#include "dht.h"
#include "epd.h"
#include "gui.h"
#include "ntp.h"
#include "news.h"
#include "scrn.h"
#include "wifi.h"
#include "stock.h"

const static char *TAG = "app";

void app_main(void)
{
	struct stock_data stock;
	stock.prices_maxlen = 90; /* days */
	stock.prices = malloc(sizeof(int) * stock.prices_maxlen);

	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_connect();

	stock_init();

	for (;;) {
		stock_get_data(&stock);

		ESP_LOGI(TAG, "stock: %s, price ref: %d", stock.ticker, stock.price_ref);

		for (int i = 0; i < stock.prices_len; i++) {
			ESP_LOGI(TAG, "price: %d", stock.prices[i]);
		}

		vTaskDelay(1 * 30 * 1000 / portTICK_PERIOD_MS);	
	}
}

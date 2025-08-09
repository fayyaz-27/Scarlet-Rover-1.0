// rx.c
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "rx.h"

static const char *TAG = "RX_MODULE";

// Callback function for receiving data
static void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (recv_info == NULL || data == NULL || len <= 0) return;

    const uint8_t *mac = recv_info->src_addr;
    ESP_LOGI(TAG, "Received from %02X:%02X:%02X:%02X:%02X:%02X => %d",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], data[0]);
}

void espnow_rx_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));
    ESP_LOGI(TAG, "ESP-NOW RX initialized.");
}

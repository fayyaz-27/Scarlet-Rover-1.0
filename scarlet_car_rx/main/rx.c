// Project Name: ESP-NOW Driver Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "rx.h"

static const char *TAG = "RX_MODULE";
static uint8_t latest_rx_data = 0;  // stores latest value

// Callback function for receiving data
static void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (recv_info == NULL || data == NULL || len <= 0) return;

    latest_rx_data = data[0];  // Save received data

    const uint8_t *mac = recv_info->src_addr;
    ESP_LOGI(TAG, "Received from %02X:%02X:%02X:%02X:%02X:%02X => %d",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], latest_rx_data);
}

void espnow_rx_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));
    ESP_LOGI(TAG, "ESP-NOW RX initialized.");
}


uint8_t get_latest_rx_data(void) {
    return latest_rx_data;
}

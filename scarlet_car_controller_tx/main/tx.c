// tx.c
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "tx.h"

static const char *TAG = "TX_MODULE";
static uint8_t peer_addr[ESP_NOW_ETH_ALEN];

void espnow_tx_init(const uint8_t *peer_mac) {
    // Store peer MAC
    memcpy(peer_addr, peer_mac, ESP_NOW_ETH_ALEN);

    ESP_ERROR_CHECK(esp_now_init());

    // Add peer
    esp_now_peer_info_t peer = {
        .channel = 0,
        .ifidx = ESP_IF_WIFI_STA,
        .encrypt = false,
    };
    memcpy(peer.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));

    ESP_LOGI(TAG, "ESP-NOW TX initialized.");
}

void espnow_tx_send(uint8_t data) {
    esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Sent: %d", data);
    } else {
        ESP_LOGE(TAG, "Send error: %s", esp_err_to_name(result));
    }
}

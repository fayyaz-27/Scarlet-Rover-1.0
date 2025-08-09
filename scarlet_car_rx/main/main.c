// Project Name: Main Application Code, Scarlet Car Rx 1.0
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/mcpwm_prelude.h" 

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"

#include "soc/mcpwm_periph.h"
#include "driver/gpio.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_now.h"

#include "rx.h"
#include "mcpwm_driver.h"
#include "mcpwm_driver_2.h"
#include "ir_driver.h"


static const char *TAG = "MAIN_RX";

void app_main(void) {
    // Basic init
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Init ESP-NOW RX module
    espnow_rx_init();

    // Print MAC address (use in TX)
    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
    ESP_LOGI(TAG, "Receiver MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);


    uint8_t gpio_num_fwd = 4;
    uint8_t gpio_num_rev = 5;
    uint8_t gpio_num_fwd_2 = 13;
    uint8_t gpio_num_rev_2 = 15;

    uint8_t gpio_ir_read = 12;

    uint32_t freq_hz = 50;

    gpio_set_direction(2, GPIO_MODE_OUTPUT);

    gpio_set_direction(gpio_ir_read, GPIO_MODE_INPUT);

    ir_init(gpio_ir_read);

    mcpwm_driver_init(gpio_num_fwd, gpio_num_rev, freq_hz, 0.0f);
    mcpwm_motor2_init(gpio_num_fwd_2, gpio_num_rev_2, freq_hz, 0.0f);

    while(1){
        uint8_t data = get_latest_rx_data();
        printf("%d\n", data);
        
        float duty_cycle = 100.0f;

        if(data == 3){

            gpio_set_level(2, 1);
            ESP_LOGI("MCPWM", "FORWARD");
            mcpwm_set_motor_direction(true, duty_cycle);
            mcpwm_motor2_set_direction(true, duty_cycle);
        }
        else if(data == 4){
                    
            gpio_set_level(2, 0);
            ESP_LOGI("MCPWM", "REVERSE");
            mcpwm_set_motor_direction(false, duty_cycle);
            mcpwm_motor2_set_direction(false, duty_cycle);
        }
        else if(data == 1){
            
            gpio_set_level(2, 1);
            ESP_LOGI("MCPWM", "Right");
            mcpwm_set_motor_direction(true, duty_cycle);
            mcpwm_motor2_set_direction(false, duty_cycle);
        }
        else if(data == 2){

            gpio_set_level(2, 0);
            ESP_LOGI("MCPWM", "Left");
            mcpwm_set_motor_direction(false, duty_cycle);
            mcpwm_motor2_set_direction(true, duty_cycle);
        }
        else{
            gpio_set_level(2, 0);
            ESP_LOGI("MCPWM", "STOP");
            mcpwm_set_motor_direction(true, 0.0f);
            mcpwm_motor2_set_direction(true, 0.0f);
            mcpwm_set_motor_direction(false, 0.0f);
            mcpwm_motor2_set_direction(false, 0.0f);
        }

        uint8_t status = read_ir(gpio_ir_read);

        if(status == 0){
            
            ESP_LOGI("MCPWM", "REVERSE");
            mcpwm_set_motor_direction(false, duty_cycle);
            mcpwm_motor2_set_direction(true, duty_cycle);

            vTaskDelay(pdMS_TO_TICKS(1000));

            ESP_LOGI("MCPWM", "STOP");
            mcpwm_set_motor_direction(true, 0.0f);
            mcpwm_motor2_set_direction(true, 0.0f);
            mcpwm_set_motor_direction(false, 0.0f);
            mcpwm_motor2_set_direction(false, 0.0f);

        }
        vTaskDelay(pdMS_TO_TICKS(100));

    }

}

// File: MPU_6050 Test Code
// Author: Fayyaz Shaikh
// Email ID: fayyaz.shaikh24@spit.ac.in
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/

#include<stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_macros.h"
#include "driver/gpio.h"
#include <math.h>
#include "esp_system.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "my_6050_driver.h"
#include "e32_lora_driver.h"
#include "esp_now.h"
#include "tx.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"



#define TAG "MPU6050"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static const char *TAG_LORA = "E32 LoRa";

uint8_t rx_mac[ESP_NOW_ETH_ALEN] = {0x4C, 0xEB, 0xD6, 0x7B, 0x28, 0x5C};;


void app_main(){

    esp_err_t status;
    const uart_port_t uart_num = UART_NUM_1;

    
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize ESP-NOW TX with peer MAC
    espnow_tx_init(rx_mac);

//     // Simulate real-time changing value
//     uint8_t data = 27;
    
    status = my_mpu6050_init();
    if (status != ESP_OK){
        ESP_LOGE(TAG, "MPU6050 initialization failed with error code: %d", status);
    }
    else {
        ESP_LOGI(TAG, "MPU6050 initialization successful with code: %d", status);
    }

    uint8_t who_am_i = my_mpu6050_who_am_i();
    ESP_LOGI(TAG, "WHO AM I Register Value: 0x%02X", who_am_i);

    esp_err_t status_lora = e32_init();
     
    if (status_lora == ESP_OK){
        ESP_LOGI(TAG_LORA, "E32 LoRa Initialization Successful !");
    } 
    else {
        ESP_LOGE(TAG_LORA, "E32 LoRa Initialization Failed !");
    }

    vTaskDelay(pdMS_TO_TICKS(500));


    while(1){
        float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, temp;
        uint8_t gesture_value = 0;


        mpu6050_read_acceleration(&acc_x, &acc_y, &acc_z);
        mpu6050_read_gyroscope(&gyro_x, &gyro_y, &gyro_z);
        mpu6050_read_temperature(&temp);

        ESP_LOGI(TAG, "Acceleration:-  X-Axis: %.2f, Y-Axis: %.2f, Z-Axis: %.2f", acc_x, acc_y, acc_z);
        ESP_LOGI(TAG, "Gyroscope:- X-Axis: %.2f, Y-Axis: %.2f, Z-Axis: %.2f", gyro_x, gyro_y, gyro_z);
        ESP_LOGI(TAG, "Temperature:- %.2f", temp);

        if(acc_x > 0.50){
                gesture_value = 1;
                printf("Device Left Tilt: %d\n", gesture_value);
        }
        else if(acc_x < -0.50){
                gesture_value = 2;
                printf("Device Right Tilt: %d\n", gesture_value);
        }
        else if((acc_x == 0.00) | (acc_y == 0.00)){
                gesture_value = 0;
                printf("No Tilt: %d\n", gesture_value);
        }
        else if(acc_y > 0.50){
                gesture_value = 3;
                printf("Reverse Tilt: %d\n", gesture_value);
        }
        else if(acc_y < -0.50){
                gesture_value = 4;
                printf("Forward Tilt: %d\n", gesture_value);
        }

        espnow_tx_send(gesture_value);

        /* if(acc_z > 0.30){
                printf("Backward Leg\n");
        }
        if(acc_z < -0.30){
                printf("Front Leg\n");
        }*/

        // char gesture_str[4];        // enough to hold "255\0"

        // sprintf(gesture_str, "%d", gesture_value);

        // uart_write_bytes(uart_num, gesture_str, strlen(gesture_str));
        // uart_wait_tx_done(uart_num, pdMS_TO_TICKS(100));

        // status_lora = e32_transmit(e32_string);
        // if(status_lora == ESP_OK){
        //     ESP_LOGI(TAG_LORA, "E32 Data Sent");
        // }
        // else{
        //     ESP_LOGE(TAG_LORA, "E32 Data failed to send");
        // }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
// Project Name: MCPWM Motor 2 Driver Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include "driver/mcpwm_prelude.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"


#include "mcpwm_driver_2.h"

#define MCPWM_TIMER_RESOLUTION_HZ 1000000


// Motor 2-specific handles
static mcpwm_timer_handle_t timer2 = NULL;
static mcpwm_oper_handle_t oper2 = NULL;
static mcpwm_gen_handle_t gen2_fwd = NULL;
static mcpwm_gen_handle_t gen2_rev = NULL;
static mcpwm_cmpr_handle_t comp2_fwd = NULL;
static mcpwm_cmpr_handle_t comp2_rev = NULL;

static uint32_t period_ticks_motor2 = 0;

esp_err_t mcpwm_motor2_set_direction(bool forward, float duty_cycle) {
    if(duty_cycle > 100.0f){
        duty_cycle = 100.0f;
    }
    if(duty_cycle < 0.0f){
        duty_cycle = 0.0f;
    }

    uint32_t duty_ticks = (uint32_t)((duty_cycle / 100.0f) * period_ticks_motor2);
    esp_err_t ret;

    if(forward){
        ret = mcpwm_comparator_set_compare_value(comp2_fwd, duty_ticks);
        if(ret != ESP_OK){
            return ret;
        }
        ret = mcpwm_comparator_set_compare_value(comp2_rev, 0);
    }
    else{
        ret = mcpwm_comparator_set_compare_value(comp2_fwd, 0);
        if(ret != ESP_OK){
            return ret;
        }
        ret = mcpwm_comparator_set_compare_value(comp2_rev, duty_ticks);
    }

    if(ret != ESP_OK){
        ESP_LOGE("MCPWM", "Failed to update duty cycle: %s", esp_err_to_name(ret));
    }
    return ret;

}

esp_err_t mcpwm_motor2_init(uint8_t fwd_gpio, uint8_t rev_gpio, uint32_t freq_hz, float duty_cycle){
    
esp_err_t ret;

mcpwm_timer_config_t timer_config = {
    .group_id = 1,
    .intr_priority = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = 1000000,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = 1000000 / freq_hz,
    
};

ret = mcpwm_new_timer(&timer_config, &timer2);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create timer for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

period_ticks_motor2 = timer_config.period_ticks;


mcpwm_operator_config_t oper_config = {
    .group_id = 1,
    .intr_priority = 0,
 
};

ret = mcpwm_new_operator(&oper_config, &oper2);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create operator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_operator_connect_timer(oper2, timer2);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to connect operator to timer: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_generator_config_t gen_config_fwd = {
    .gen_gpio_num = fwd_gpio,

};

ret = mcpwm_new_generator(oper2, &gen_config_fwd, &gen2_fwd);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create generator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_generator_config_t gen_config_rev = {
    .gen_gpio_num = rev_gpio,

};

ret = mcpwm_new_generator(oper2, &gen_config_rev, &gen2_rev);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create generator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_comparator_config_t comp_config = {
    .intr_priority = 0,
};

ret = mcpwm_new_comparator(oper2, &comp_config, &comp2_fwd);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create comparator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_new_comparator(oper2, &comp_config, &comp2_rev);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create comparator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}


// FORWARD generator
mcpwm_gen_timer_event_action_t fwd_timer_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .event = MCPWM_TIMER_EVENT_EMPTY,
    .action = MCPWM_GEN_ACTION_HIGH
};
ret = mcpwm_generator_set_action_on_timer_event(gen2_fwd, fwd_timer_action);
if (ret != ESP_OK) {
    ESP_LOGE("MCPWM", "Failed to set forward timer event: %s", esp_err_to_name(ret));
    return ret;
}
mcpwm_gen_compare_event_action_t fwd_compare_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .comparator = comp2_fwd,
    .action = MCPWM_GEN_ACTION_LOW
};
ret = mcpwm_generator_set_action_on_compare_event(gen2_fwd, fwd_compare_action);
if (ret != ESP_OK) {
    ESP_LOGE("MCPWM", "Failed to set forward compare event: %s", esp_err_to_name(ret));
    return ret;
}
// REVERSE generator
mcpwm_gen_timer_event_action_t rev_timer_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .event = MCPWM_TIMER_EVENT_EMPTY,
    .action = MCPWM_GEN_ACTION_HIGH
};
ret = mcpwm_generator_set_action_on_timer_event(gen2_rev, rev_timer_action);
if (ret != ESP_OK) {
    ESP_LOGE("MCPWM", "Failed to set reverse timer event: %s", esp_err_to_name(ret));
    return ret;
}
mcpwm_gen_compare_event_action_t rev_compare_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .comparator = comp2_rev,
    .action = MCPWM_GEN_ACTION_LOW
};
ret = mcpwm_generator_set_action_on_compare_event(gen2_rev, rev_compare_action);
if (ret != ESP_OK) {
    ESP_LOGE("MCPWM", "Failed to set reverse compare event: %s", esp_err_to_name(ret));
    return ret;
}


uint32_t period = timer_config.period_ticks;
uint32_t duty_ticks = (uint32_t)((duty_cycle/100.0f)*period);

ret = mcpwm_comparator_set_compare_value(comp2_fwd, 0);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to set Compare value: %s", esp_err_to_name(ret));
    return ret;
}
ret = mcpwm_comparator_set_compare_value(comp2_rev, 0);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to set Compare value: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_timer_enable(timer2);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Timer Enable Failed: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_timer_start_stop(timer2, MCPWM_TIMER_START_NO_STOP);
    return ret;

}

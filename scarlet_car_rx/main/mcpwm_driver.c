// Project Name: MCPWM Motor 1 Driver Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239

// 1. Create timer
// 2. Create operator
// 3. Connect operator to timer
// 4. Create generator from operator



#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
// #include "driver/mcpwm.h"
#include "driver/mcpwm_prelude.h"  // new driver

#include "soc/mcpwm_periph.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "mcpwm_driver.h"

#define MCPWM_TIMER_RESOLUTION_HZ 1000000



mcpwm_timer_handle_t timer = NULL;
mcpwm_oper_handle_t oper = NULL;
mcpwm_gen_handle_t gen_fwd = NULL;
mcpwm_gen_handle_t gen_rev = NULL;
mcpwm_cmpr_handle_t comp_fwd = NULL;
mcpwm_cmpr_handle_t comp_rev = NULL;


static uint32_t period_ticks_internal = 0;


esp_err_t mcpwm_set_motor_direction(bool forward, float duty_cycle){
    if(duty_cycle > 100.0f){
        duty_cycle = 100.0f;
    }
    if(duty_cycle < 0.0f){
        duty_cycle = 0.0f;
    }

    uint32_t duty_ticks = (uint32_t)((duty_cycle / 100.0f) * period_ticks_internal);
    esp_err_t ret;

    if(forward){
        ret = mcpwm_comparator_set_compare_value(comp_fwd, duty_ticks);
        if(ret != ESP_OK){
            return ret;
        }
        ret = mcpwm_comparator_set_compare_value(comp_rev, 0);
    }
    else{
        ret = mcpwm_comparator_set_compare_value(comp_fwd, 0);
        if(ret != ESP_OK){
            return ret;
        }
        ret = mcpwm_comparator_set_compare_value(comp_rev, duty_ticks);
    }

    if(ret != ESP_OK){
        ESP_LOGE("MCPWM", "Failed to update duty cycle: %s", esp_err_to_name(ret));
    }
    return ret;
}

// esp_err_t mcpwm_set_duty_cycle(float duty_cycle){
//     if(duty_cycle > 100.0f){
//         duty_cycle = 100.0f;
//     }
//     if(duty_cycle < 0.0f){
//         duty_cycle = 0.0f;
//     }

//     uint32_t duty_ticks = (uint32_t)((duty_cycle/100.0f)*period_ticks_internal);

//     esp_err_t ret = mcpwm_comparator_set_compare_value(comp, duty_ticks);
//     if(ret != ESP_OK){
//         ESP_LOGE("MCPWM", "Failed to set Duty Ticks: %s", esp_err_to_name(ret));
//     }
//     return ESP_OK;
// }


esp_err_t mcpwm_driver_init(uint8_t fwd_gpio_num, uint8_t rev_gpio_num, uint32_t freq_hz, float duty_cycle){

esp_err_t ret;

mcpwm_timer_config_t timer_config = {
    .group_id = 0,
    .intr_priority = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = 1000000,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = 1000000 / freq_hz,
    // .update_period_on_empty = true,
    // .update_period_on_sync = false
};

ret = mcpwm_new_timer(&timer_config, &timer);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create timer for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

period_ticks_internal = timer_config.period_ticks;


mcpwm_operator_config_t oper_config = {
    .group_id = 0,
    .intr_priority = 0,
    // .update_gen_action_on_tez = 
    // .update_gen_action_on_tep = 
    // .update_gen_action_on_sync = 
    // .update_dead_time_on_tez =
    // .update_dead_time_on_tep = 
    // .update_dead_time_on_sync = 
};

ret = mcpwm_new_operator(&oper_config, &oper);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create operator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_operator_connect_timer(oper, timer);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to connect operator to timer: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_generator_config_t gen_config_fwd = {
    .gen_gpio_num = fwd_gpio_num,
    // .invert_pwm = false,
    // .pull_up = false,
    // .pull_down = false,
};

ret = mcpwm_new_generator(oper, &gen_config_fwd, &gen_fwd);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create generator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_generator_config_t gen_config_rev = {
    .gen_gpio_num = rev_gpio_num,
    // .invert_pwm = false,
    // .pull_up = false,
    // .pull_down = false,
};

ret = mcpwm_new_generator(oper, &gen_config_rev, &gen_rev);

if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create generator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

mcpwm_comparator_config_t comp_config = {
    .intr_priority = 0,
};

ret = mcpwm_new_comparator(oper, &comp_config, &comp_fwd);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create comparator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_new_comparator(oper, &comp_config, &comp_rev);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to create comparator for MCPWM: %s", esp_err_to_name(ret));
    return ret;
}

//forward

// FORWARD generator
mcpwm_gen_timer_event_action_t fwd_timer_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .event = MCPWM_TIMER_EVENT_EMPTY,
    .action = MCPWM_GEN_ACTION_HIGH
};
ret = mcpwm_generator_set_action_on_timer_event(gen_fwd, fwd_timer_action);

mcpwm_gen_compare_event_action_t fwd_compare_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .comparator = comp_fwd,
    .action = MCPWM_GEN_ACTION_LOW
};
ret = mcpwm_generator_set_action_on_compare_event(gen_fwd, fwd_compare_action);




// ret = mcpwm_generator_set_action_on_timer_event(gen_fwd, 
//       MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH), 
//       MCPWM_GEN_TIMER_EVENT_ACTION_END);

// if(ret != ESP_OK){
//     ESP_LOGE("MCPWM", "Failed to set Timer event: %s", esp_err_to_name(ret));
//     return ret;
// }
// ret = mcpwm_generator_set_action_on_compare_event(gen_fwd, 
//       MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comp_fwd, MCPWM_GEN_ACTION_LOW),
//       MCPWM_GEN_COMPARE_EVENT_ACTION_END);

// if(ret != ESP_OK){
//     ESP_LOGE("MCPWM", "Failed to set Compare event: %s", esp_err_to_name(ret));
//     return ret;
// }

//reverse

// REVERSE generator
mcpwm_gen_timer_event_action_t rev_timer_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .event = MCPWM_TIMER_EVENT_EMPTY,
    .action = MCPWM_GEN_ACTION_HIGH
};
ret = mcpwm_generator_set_action_on_timer_event(gen_rev, rev_timer_action);

mcpwm_gen_compare_event_action_t rev_compare_action = {
    .direction = MCPWM_TIMER_DIRECTION_UP,
    .comparator = comp_rev,
    .action = MCPWM_GEN_ACTION_LOW
};
ret = mcpwm_generator_set_action_on_compare_event(gen_rev, rev_compare_action);


// ret = mcpwm_generator_set_action_on_timer_event(gen_rev, 
//       MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH), 
//       MCPWM_GEN_TIMER_EVENT_ACTION_END);

// if(ret != ESP_OK){
//     ESP_LOGE("MCPWM", "Failed to set Timer event: %s", esp_err_to_name(ret));
//     return ret;
// }
// ret = mcpwm_generator_set_action_on_compare_event(gen_rev, 
//       MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comp_rev, MCPWM_GEN_ACTION_LOW),
//       MCPWM_GEN_COMPARE_EVENT_ACTION_END);

// if(ret != ESP_OK){
//     ESP_LOGE("MCPWM", "Failed to set Compare event: %s", esp_err_to_name(ret));
//     return ret;
// }


uint32_t period = timer_config.period_ticks;
uint32_t duty_ticks = (uint32_t)((duty_cycle/100.0f)*period);

ret = mcpwm_comparator_set_compare_value(comp_fwd, 0);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to set Compare value: %s", esp_err_to_name(ret));
    return ret;
}
ret = mcpwm_comparator_set_compare_value(comp_rev, 0);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Failed to set Compare value: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_timer_enable(timer);
if(ret != ESP_OK){
    ESP_LOGE("MCPWM", "Timer Enable Failed: %s", esp_err_to_name(ret));
    return ret;
}

ret = mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP);
    return ret;
}


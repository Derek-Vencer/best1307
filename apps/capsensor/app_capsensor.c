/***************************************************************************
 *
 * Copyright 2022-2023 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef CHIP_SUBSYS_SENS
#include "app_capsensor.h"
#include "stdint.h"
#include "touch_wear_core.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(capsensor_driver)
#include "capsensor_driver.h"
#if defined(CAPSENSOR_AT_SENS)
#include "app_sensor_hub.h"
#endif
#include "hal_trace.h"
#include "app_key.h"
#include "stdint.h"
#include "string.h"
#include "tgt_hardware_capsensor.h"
#include "capsensor_driver_best1307.h"

#include "hal_key.h"

#ifdef CUSTOMER_APP_BOAT
#include "app_tota.h"
#include "apps.h"
static bool inear_detect_status =false;
void app_inear_detect_set_on_ear(void)
{
    inear_detect_status = true;
    TRACE(1, "[UICAP]%s, ear = %d\n", __func__, inear_detect_status);
}
void app_inear_detect_set_off_ear(void)
{
    inear_detect_status = false;
    TRACE(1, "[UICAP]%s, ear = %d\n", __func__, inear_detect_status);
}
bool app_inear_get_status(void)
{
    TRACE(1, "[UICAP]%s, ear = %d\n", __func__, inear_detect_status);
    return inear_detect_status;
}
#endif //#ifdef CUSTOMER_APP_BOAT

void app_capsensor_click_event(uint8_t key_event);

#if defined(CAPSENSOR_AT_SENS)
static void app_mcu_sensor_hub_transmit_touch_no_rsp_cmd_handler(uint8_t* ptr, uint16_t len)
{
    app_core_bridge_send_data_without_waiting_rsp(MCU_SENSOR_HUB_TASK_CMD_TOUCH_REQ_NO_RSP, ptr, len);
}

static void app_mcu_sensor_hub_touch_no_rsp_cmd_received_handler(uint8_t* ptr, uint16_t len)
{
    APP_KEY_STATUS status = *((APP_KEY_STATUS *)ptr);
    TRACE(0, "Get touch no rsp command from sensor hub core, status.event: %d", status.event);
    app_capsensor_click_event(status.event);
}

static void app_mcu_sensor_hub_touch_no_rsp_cmd_tx_done_handler(uint16_t cmdCode,
    uint8_t* ptr, uint16_t len)
{
    TRACE(0, "cmdCode 0x%x tx done", cmdCode);
}

CORE_BRIDGE_TASK_COMMAND_TO_ADD(MCU_SENSOR_HUB_TASK_CMD_TOUCH_REQ_NO_RSP,
                                "touch no rsp req to sensor hub core",
                                app_mcu_sensor_hub_transmit_touch_no_rsp_cmd_handler,
                                app_mcu_sensor_hub_touch_no_rsp_cmd_received_handler,
                                0,
                                NULL,
                                NULL,
                                app_mcu_sensor_hub_touch_no_rsp_cmd_tx_done_handler);
#endif


uint8_t app_capsensor_ch_num_get(void)
{
    return CAP_CHNUM;
}

uint8_t app_capsensor_conversion_num_get(void)
{
    return CAP_REPNUM;
}

uint16_t app_capsensor_samp_fs_get(void)
{
    return CAP_SAMP_FS;
}

uint8_t app_capsensor_ch_map_get(void)
{
    return (CAP_CH0 | CAP_CH1 | CAP_CH2 | CAP_CH3 | CAP_CH4);
}

void app_capsensor_suspend(void)
{
    capsensor_suspend();
}

void app_capsensor_resume(void)
{
    capsensor_resume();
}

void app_capsensor_close(void)
{
    capsensor_close();
}

void app_capsensor_open(void)
{
    capsensor_open();
}

void app_capsensor_driver_baseline_dr(uint32_t* baseline_value_p)
{
    capsensor_driver_baseline_dr(baseline_value_p);
}

void app_capsensor_driver_baseline_reg_read(uint32_t* baseline_value_p)
{
    capsensor_driver_baseline_reg_read(baseline_value_p);
}

//touch config
const TouchConfig cap_touch_config=
{
    .cap_keydown_th                 = CAP_KEYDOWN_TH,
    .cap_keyup_th                   = CAP_KEYUP_TH,
#ifdef CAPSENSOR_SLIDE
    .slide_channel_0                = WORKING_CH0,
    .slide_channel_1                = WORKING_CH1,
    .slide_channel_2                = WORKING_CH2,
#else
    .slide_channel_0                = WORKING_CH1,
    .slide_channel_1                = WORKING_NULL,
    .slide_channel_2                = WORKING_NULL,
#endif
    .cap_key_click_max_duration     = CAP_KEY_CLICK_MAX_DURATION,// 350m
    .cap_key_click_min_duration     = CAP_KEY_CLICK_MIN_DURATION,// 50ms
    .cap_key_double_interval        = CAP_KEY_DOUBLE_INTERVAL ,  // double key interval
    .cap_key_triple_interval        = CAP_KEY_TRIPLE_INTERVAL ,  // triple key interval
    .cap_key_ultra_interval         = CAP_KEY_ULTRA_INTERVAL  ,  // ultra  key interval
    .cap_key_rampage_interval       = CAP_KEY_RAMPAGE_INTERVAL,  // rampage key interval
    .cap_key_slide_up_interval      = CAP_KEY_SLIDE_UP_INTERVAL, // slide up key interval
    .cap_key_long_interval          = CAP_KEY_LONG_INTERVAL    , // 1000ms
    .cap_key_longlong_interval      = CAP_KEY_LONGLONG_INTERVAL, // 5000ms
    .cap_dc_reset_interval          = CAP_DC_RESET_INTERVAL    , // 20000ms
};

//Wear confign
const WearConfig cap_wear_config=
{
    .wear_detect_channel_0             = WORKING_CH2,
    .wear_detect_channel_1             = WORKING_CH4,
    .wear_reference_channel_0          = WORKING_CH0,
    .wear_reference_channel_1          = WORKING_CH3,
    .cap_wear_threshold_high1          = CAP_WEAR_THRESHOLD_HIGH1,
    .cap_wear_threshold_high2          = CAP_WEAR_THRESHOLD_HIGH2,
    .cap_wear_threshold_low1           = CAP_WEAR_THRESHOLD_LOW1,
    .cap_wear_threshold_low2           = CAP_WEAR_THRESHOLD_LOW2,
    .cap_wear_twopad_threshold1        = CAP_WEAR_TWOPAD_THRESHOLD1,
    .cap_wear_twopad_state_win1        = CAP_WEAR_TWOPAD_STATE_WIN1,
    .cap_wear_ear_on_state_win         = CAP_WEAR_EAR_ON_STATE_WIN,
    .cap_wear_ear_off_state_win        = CAP_WEAR_EAR_OFF_STATE_WIN,
    .cap_wear_offset0_update_threshold = CAP_WEAR_OFFSET0_UPDATE_THRESHOLD,
    .cap_wear_offset1_update_threshold = CAP_WEAR_OFFSET1_UPDATE_THRESHOLD,
    .cap_wear_jump_threshold1          = CAP_WEAR_JUMP_THRESHOLD1,
    .cap_wear_jump_threshold2          = CAP_WEAR_JUMP_THRESHOLD2,
    .cap_wear_on_delay_count           = CAP_WEAR_ON_DELAY_COUNT,
    .cap_wear_on_delay_mse_th          = CAP_WEAR_ON_DELAY_MSE_TH,
};

/***************************************************************************
 * @brief app_capsensor_click_event function
 *
 * @param key_event touch or wear event
 ***************************************************************************/
void app_capsensor_click_event(uint8_t key_event)
{
    APP_KEY_STATUS status;

    status.event = key_event;

    TRACE(1, "[%s]:%d",  __func__, status.event);

    switch (status.event)
    {
        case CAP_KEY_EVENT_UP:
            TRACE(1, "capsensor state= key up\n");
            status.code = HAL_KEY_CODE_PWR;
            status.event = APP_KEY_EVENT_UP;
            break;

        case CAP_KEY_EVENT_DOWN:
            TRACE(1, "capsensor state= key down\n");
            status.code = HAL_KEY_CODE_PWR;
            status.event = APP_KEY_EVENT_DOWN;
            break;

        case CAP_KEY_EVENT_UPSLIDE:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= up slide\n");
            status.event = APP_KEY_EVENT_UPSLIDE;
            break;

        case CAP_KEY_EVENT_DOWNSLIDE:
            TRACE(1, "capsensor state= down slide\n");
            status.event = APP_KEY_EVENT_DOWNSLIDE;
            break;

        case CAP_KEY_EVENT_CLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= single click\n");
            status.event = APP_KEY_EVENT_CLICK;
            break;

        case CAP_KEY_EVENT_DOUBLECLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= double click\n");
            status.event = APP_KEY_EVENT_DOUBLECLICK;
            break;

        case CAP_KEY_EVENT_TRIPLECLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= triple click\n");
            status.event = APP_KEY_EVENT_TRIPLECLICK;
            break;

        case CAP_KEY_EVENT_ULTRACLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= ultra click\n");
            status.event = APP_KEY_EVENT_ULTRACLICK;
            break;

        case CAP_KEY_EVENT_RAMPAGECLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= rampage click\n");
            status.event = APP_KEY_EVENT_RAMPAGECLICK;
            break;

        case CAP_KEY_EVENT_SIXCLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= six click\n");
            break;

        case CAP_KEY_EVENT_SEVENCLICK:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= seven click\n");
            break;

        case CAP_KEY_EVENT_LONGPRESS:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= long press\n");
            status.event = APP_KEY_EVENT_LONGPRESS;
            break;

        case CAP_KEY_EVENT_LONGLONGPRESS:
            status.code = HAL_KEY_CODE_PWR;
            TRACE(1, "capsensor state= long  long press\n");
            status.event = APP_KEY_EVENT_LONGLONGPRESS;
            break;

        case CAP_KEY_EVENT_CLICK_AND_LONGPRESS:
            TRACE(1, "capsensor state= click and long press\n");
            status.code = HAL_KEY_CODE_PWR;
            break;

        case CAP_KEY_EVENT_CLICK_AND_LONGLONGPRESS:
            TRACE(1, "capsensor state= click and longlongpress\n");
            status.code = HAL_KEY_CODE_PWR;
            break;

        case CAP_KEY_EVENT_DOUBLECLICK_AND_LONGLONGPRESS:
            TRACE(1, "capsensor state= double click and longlongpress\n");
            break;

        case CAP_KEY_EVENT_TRIPLECLICK_AND_LONGLONGPRESS:
            TRACE(1, "capsensor state= triple click and longlongpress\n");
            status.code = HAL_KEY_CODE_PWR;
            break;

        default:
            break;
    }
}

/***************************************************************************
 * @brief  app_mcu_core_capsensor_init function
 *
 ***************************************************************************/
void app_mcu_core_capsensor_init(void)
{
    int ret = -1;
    ret = register_capsensor_click_event_callback(app_capsensor_click_event);
    if (ret) {
        TRACE(1,"register_capsensor_click_event_callback failed:%d\n", ret);
    }

    ret = capsensor_channel_number_get_callback(app_capsensor_ch_num_get);
    if (ret) {
        TRACE(1,"capsensor_channel_number_get_callback failed:%d\n", ret);
    }

    ret = capsensor_conversion_number_get_callback(app_capsensor_conversion_num_get);
    if (ret) {
        TRACE(1,"capsensor_conversion_number_get_callback failed:%d\n", ret);
    }

    ret = capsensor_channel_map_get_callback(app_capsensor_ch_map_get);
    if (ret) {
        TRACE(1,"capsensor_channel_map_get_callback failed:%d\n", ret);
    }

    ret = capsensor_samp_fs_get_callback(app_capsensor_samp_fs_get);
    if (ret) {
        TRACE(1,"capsensor_samp_fs_get_callback failed:%d\n", ret);
    }

    capsensor_driver_init();

#ifdef CAPSENSOR_AT_MCU
    capsensor_sens2mcu_irq_set();
    cap_sensor_core_thread_init();
#endif
}

#endif


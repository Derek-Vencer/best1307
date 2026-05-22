/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "bluetooth_bt_api.h"
#include "hal_cmu.h"
#include "app_tota_cmd_code.h"
#include "app_tota.h"
#include "app_tota_cmd_handler.h"
#include "cmsis.h"
#include "app_hfp.h"
#include "app_key.h"
#include "app_tota_general.h"
#include "app_spp_tota.h"
#include "nvrecord_ble.h"
#include "app_ibrt_rssi.h"
#include "app_tota_if.h"
#include "app_tota_common.h"
#if defined(TOTA_EQ_TUNING)
#include "hal_cmd.h"
#endif
#ifdef IBRT
#include "earbud_ux_api.h"
#endif
#ifdef CUSTOMER_APP_BOAT
#include "app_tws_ctrl_thread.h"
#include "apps.h"
#include "app_bt_stream.h"
#include "app_utils.h"
#include "app_ibrt_customif_cmd.h"
#include "app_ibrt_customif_ui.h"
#include "app_audio_control.h"
#ifdef ANC_APP
#include "app_anc.h"
#endif

#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
#include "app_voice_assist_custom_leak_detect.h"
#endif

#include "audio_policy.h"
#include "app_bt_media_manager.h"
#include "app_a2dp.h"

#include "nvrecord_extension.h"
#include "nvrecord_env.h"

struct nvrecord_env_t *p_nvrecord_env;
#ifdef ALGO_INFO_SYNC_EN
#include "app_thread.h"
#endif

#include "bes_gap_api.h"
#include "app_capsensor.h"
#define TOTA_CRC_CHECK  0x54534542
#define LEN_OF_IMAGE_TAIL_TO_FINDKEY_WORD    512
#endif

#if (TOTA_GENERAL_ENABLE)
/*
** general info struct
**  ->  bt  name
**  ->  ble name
**  ->  bt  local/peer addr
**  ->  ble local/peer addr
**  ->  ibrt role
**  ->  crystal freq
**  ->  xtal fcap
**  ->  bat volt/level/status
**  ->  fw version
**  ->  ear location
**  ->  rssi info
*/

/*------------------------------------------------------------------------------------------------------------------------*/
typedef struct{
    uint32_t    address;
    uint32_t    length;
}TOTA_DUMP_INFO_STRUCT_T;

#ifdef CUSTOMER_APP_BOAT
typedef struct{
    uint32_t    crc;            //fw crc, generate by generate_crc32_of_image.py
    uint8_t     version[4];     //fw version
    uint8_t     build_date[32]; //fw build data,auto generate by compiler when build
}TOTA_CRC_CHECK_STRUCT_T;

static BUTTON_SET_EVENT_INFO_T button_event_info_left[4];
static BUTTON_SET_EVENT_INFO_T button_event_info_right[4];

extern const char sys_build_info[];

static const char* image_info_sanity_crc_key_word = "CRC32_OF_IMAGE=0x";
static const char* image_info_build_data = "BUILD_DATE=";

static int32_t find_key_word(uint8_t* targetArray, uint32_t targetArrayLen,
    uint8_t* keyWordArray,
    uint32_t keyWordArrayLen);

static uint8_t asciiToHex(uint8_t asciiCode);
static APP_TOTA_CMD_RET_STATUS_E tota_get_sanity_crc(uint32_t *sanityCrc32);
static APP_TOTA_CMD_RET_STATUS_E tota_get_buidl_data(uint8_t *buildData);
#endif


static void general_get_flash_dump_info(TOTA_DUMP_INFO_STRUCT_T * p_flash_info);

/*
**  general info
*/
static general_info_t general_info;


/*
**  get general info
*/
static void __get_general_info();
#ifdef CUSTOMER_APP_BOAT
extern void bt_audio_updata_eq(uint8_t index);
#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
static bool start_leak_detect_flag = false;
void app_tota_start_leak_detect(void)
{
    TRACE(0,"app_tota_start_leak_detect");
    if(APP_TOTA_VIA_NOTIFICATION == tota_get_connect_path())
    {
         ///>send to slave
         tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_TOTA_LEAK_DETECT, NULL, 0);
    }
    app_sysfreq_req(APP_SYSFREQ_USER_CUSTOM_DECTET, APP_SYSFREQ_208M);
    app_voice_assist_custom_leak_detect_open();
    app_sysfreq_req(APP_SYSFREQ_USER_CUSTOM_DECTET, APP_SYSFREQ_32K);
}
bool app_tota_get_detect_flag()
{
    return start_leak_detect_flag;
}
void app_tota_set_leak_detect_value()
{
    start_leak_detect_flag = false;
}
#endif

static void button_cmd_event(uint8_t cmd)
{
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
    uint8_t device_id = app_bt_audio_get_device_for_user_action();
    TRACE(2,"[UIAPPKEY]%s, cmd = %d", __func__, cmd);
    TRACE(0,"[UIAPPKEY]%s, play_pause_flag=%d, streamming=%d", __func__, a2dp_device->a2dp_play_pause_flag, a2dp_device->a2dp_streamming);

    switch(cmd)
    {
        case BUTTON_SETTING_PRE_SONG_CMD:
        a2dp_handleKey(AVRCP_KEY_BACKWARD);
        break;
        case BUTTON_SETING_NEXT_SONG_CMD:
        a2dp_handleKey(AVRCP_KEY_FORWARD);
        break;
#if ANC_APP
        case BUTTON_SETTING_ANC_CMD:
        app_anc_loop_switch();
        break;
#endif
        case BUTTON_SETTING_CALL_CMD:
        app_ibrt_if_hf_call_redial(device_id);
        break;
        case BUTTON_SETTING_VOLUME_UP_CMD:
        app_ibrt_if_set_local_volume_up();
        break;
        case BUTTON_SETTING_VOLUME_DOWN_CMD:
        app_ibrt_if_set_local_volume_down();
        break;
        case BUTTON_SETTING_PLAY_MUSIC_CMD:
        if(a2dp_device && (a2dp_device->a2dp_play_pause_flag == 0 || a2dp_device->a2dp_streamming == false))
            a2dp_handleKey(AVRCP_KEY_PLAY);
        break;
        case BUTTON_SETTING_PAUSE_MUSIC_CMD:
        if(a2dp_device && (a2dp_device->a2dp_play_pause_flag == 0 || a2dp_device->a2dp_streamming == false))
        {
            //a2dp_handleKey(AVRCP_KEY_PLAY);
        }
        else
        {
            a2dp_handleKey(AVRCP_KEY_PAUSE);
        }
        break;
        case BUTTON_SETTING_PLAY_PAUSE_MUSIC_CMD:
        if(a2dp_device && (a2dp_device->a2dp_play_pause_flag == 0 || a2dp_device->a2dp_streamming == false))
        {
            TRACE(0,"[UIAPPKEY]play");
            a2dp_handleKey(AVRCP_KEY_PLAY);
        }
        else
        {
            TRACE(0,"[UIAPPKEY]pause");
            a2dp_handleKey(AVRCP_KEY_PAUSE);
        }
        break;
#ifdef SUPPORT_SIRI
        case BUTTON_SETTING_VOICE_ASSITANT_CMD:
        app_siri_timer_onoff(true);
        break;
#endif
        case BUTTON_SETTING_GAME_MODE_CMD:
#ifdef UI_LATENCY_EN
        app_gamemode_key_switch_process(true, 0x00, true);
#endif
        break;
        case BUTTON_SETTING_ANC_SPEAKTHRU:
#if defined(ANC_SPEAKTHRU_EN)
#if defined(ANC_APP) && defined(ALGO_INFO_SYNC_EN)
    algo_send_request(ALGO_ID_ANC, APP_ANC_MODE3, 0, 0, 0, 1);
#endif
#endif
        break;
        
        default:
        TRACE(0, "[UIAPPKEY]%s, error cmd", __func__);
        break;
    }
}

void app_tota_general_button_event_handler(APP_KEY_STATUS *status, bool is_right)
{
   TRACE(0, "button_event_info %d %d %d %d", button_event_info_left[0].lr, button_event_info_left[0].status.code, button_event_info_left[0].status.event,button_event_info_left[0].button_set_event);
    TRACE(0, "button_event_info %d %d %d %d", button_event_info_right[0].lr, button_event_info_right[0].status.code, button_event_info_right[0].status.event,button_event_info_right[0].button_set_event);
    TRACE(0, "KEY_STATUS        %d %d %d", is_right, status->code, status->event);
    struct nvrecord_env_t *p_nvrecord_env;
    nv_record_env_get(&p_nvrecord_env);
    memcpy(button_event_info_left, p_nvrecord_env->buttonInfo_left, sizeof(button_event_info_left));
    memcpy(button_event_info_right, p_nvrecord_env->buttonInfo_right, sizeof(button_event_info_right));

    for(uint8_t i=0; i<4; i++)
    {
        
        if (((button_event_info_left[i].lr == BUTTON_SETTING_LEFT_EARPHONE_CMD) && !is_right) \
            || ((button_event_info_left[i].lr == BUTTON_SETTING_RIGHT_EARPHONE_CMD) && is_right))
        {
            if(button_event_info_left[i].status.code == status->code)
            {
            TRACE(0,"%d %d", button_event_info_left[i].status.event  ,status->event);
                if (button_event_info_left[i].status.event == status->event)
                {
                    
                    button_cmd_event(button_event_info_left[i].button_set_event);
                }
            }
        }
        if (((button_event_info_right[i].lr == BUTTON_SETTING_LEFT_EARPHONE_CMD) && !is_right) \
            || ((button_event_info_right[i].lr == BUTTON_SETTING_RIGHT_EARPHONE_CMD) && is_right))
        {
            
            if(button_event_info_right[i].status.code == status->code)
            {
                
                if (button_event_info_right[i].status.event == status->event)
                {
                    
                    button_cmd_event(button_event_info_right[i].button_set_event);
                }
            }
        }
    }
}

void app_tota_general_button_event_init(void)
{
   nv_record_env_get(&p_nvrecord_env);

    if(p_nvrecord_env->init_button == 0xA5)
    {
        memcpy(button_event_info_left, p_nvrecord_env->buttonInfo_left, sizeof(button_event_info_left));
        memcpy(button_event_info_right, p_nvrecord_env->buttonInfo_right, sizeof(button_event_info_right));
        return ;
    }
    TRACE(0,"[UITOTA]%s", __func__);
    p_nvrecord_env->init_button = 0xA5;

    p_nvrecord_env->inear_left_onoff = 1;
    p_nvrecord_env->inear_right_onoff = 1;
    
#ifdef APP_MULTIPOINT_ONOFF_EN
    p_nvrecord_env->muiltipoint_onoff = 1;
#endif

#ifdef EQ_SET_CUSTOMER_EN
    p_nvrecord_env->eq_onoff = 2;
    p_nvrecord_env->eq_mode = 0;
#endif

#if defined(WEAR_DETECT_PROMPT_EN)
    p_nvrecord_env->wear_prompt_onoff = 0;
#endif

    button_event_info_left[0].lr = BUTTON_SETTING_LEFT_EARPHONE_CMD;
    button_event_info_left[0].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_left[0].status.event = APP_KEY_EVENT_CLICK;
    button_event_info_left[0].button_set_event = BUTTON_L_CLICK_DEFAULT;

    //init right button
    button_event_info_right[0].lr = BUTTON_SETTING_RIGHT_EARPHONE_CMD;
    button_event_info_right[0].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_right[0].status.event = APP_KEY_EVENT_CLICK;
    button_event_info_right[0].button_set_event = BUTTON_R_CLICK_DEFAULT;

    button_event_info_left[1].lr = BUTTON_SETTING_LEFT_EARPHONE_CMD;
    button_event_info_left[1].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_left[1].status.event = APP_KEY_EVENT_DOUBLECLICK;
    button_event_info_left[1].button_set_event = BUTTON_L_DOUBLE_DEFAULT;

    //init right button
    button_event_info_right[1].lr = BUTTON_SETTING_RIGHT_EARPHONE_CMD;
    button_event_info_right[1].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_right[1].status.event = APP_KEY_EVENT_DOUBLECLICK;
    button_event_info_right[1].button_set_event = BUTTON_R_DOUBLE_DEFAULT;

    button_event_info_left[2].lr = BUTTON_SETTING_LEFT_EARPHONE_CMD;
    button_event_info_left[2].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_left[2].status.event = APP_KEY_EVENT_TRIPLECLICK;
    button_event_info_left[2].button_set_event = BUTTON_L_TRIPLE_DEFAULT;

    //init right button
    button_event_info_right[2].lr = BUTTON_SETTING_RIGHT_EARPHONE_CMD;
    button_event_info_right[2].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_right[2].status.event = APP_KEY_EVENT_TRIPLECLICK;
    button_event_info_right[2].button_set_event = BUTTON_R_TRIPLE_DEFAULT;

    button_event_info_left[3].lr = BUTTON_SETTING_LEFT_EARPHONE_CMD;
    button_event_info_left[3].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_left[3].status.event = APP_KEY_EVENT_INITLONGPRESS;
    button_event_info_left[3].button_set_event = BUTTON_L_LONG_PRESS_DEFAULT;

    //init right button
    button_event_info_right[3].lr = BUTTON_SETTING_RIGHT_EARPHONE_CMD;
    button_event_info_right[3].status.code = BUTTON_MAP_KEY_TYPE;
    button_event_info_right[3].status.event = APP_KEY_EVENT_INITLONGPRESS;
    button_event_info_right[3].button_set_event = BUTTON_R_LONG_PRESS_DEFAULT;

    memcpy(p_nvrecord_env->buttonInfo_left, button_event_info_left, sizeof(button_event_info_left));
    memcpy(p_nvrecord_env->buttonInfo_right, button_event_info_right, sizeof(button_event_info_right));
    nv_record_env_set(p_nvrecord_env);
    nv_record_flash_flush();
}


void app_tota_general_button_event_reset(void)
{
    p_nvrecord_env->init_button = 0;
    nv_record_env_set(p_nvrecord_env);
    nv_record_flash_flush();
}

bool app_inear_get_right_status(void)
{
    TRACE(0,"[UIAPP]%s, right_status = %d", __func__, p_nvrecord_env->right_status);
    if(app_ibrt_if_is_right_side())
    {
#if defined(CAPSENSOR_ENABLE)
        return app_inear_get_status();
#else
        return 0;
#endif
    }
    else
    {
        //return p_nvrecord_env->right_status;
        return uictl.ear_another_sta;
    }
}

bool app_inear_get_left_status(void)
{
    TRACE(0,"[UIAPP]%s, left_status = %d", __func__, p_nvrecord_env->left_status);
    if(app_ibrt_if_is_left_side())
    {
#if defined(CAPSENSOR_ENABLE)
        return app_inear_get_status();
#else
        return 0;
#endif
    }
    else
    {
        //return p_nvrecord_env->left_status;
        return uictl.ear_another_sta;
    }
}

bool app_inear_get_myself_status(void)
{
#if defined(CAPSENSOR_ENABLE)
    return app_inear_get_status();
#else
    return 0;
#endif
}

static inline void sys_memcpy_swap(void *dst, const void *src, size_t length)
{
	uint8_t *pdst = (uint8_t *)dst;
	const uint8_t *psrc = (const uint8_t *)src;

	ASSERT(((psrc < pdst && (psrc + length) <= pdst) ||
		  (psrc > pdst && (pdst + length) <= psrc)),
		 "Source and destination buffers must not overlap");

	psrc += length - 1;

	for (; length > 0; length--) {
		*pdst++ = *psrc--;
	}
}

void app_tota_phone_num_to_app(uint8_t num)
{
    uint8_t resData[8]={0};
    resData[0] = APP_TOTA_GET_BT_ADDR_CMD;
    if(num != 0xFF)
        resData[1] = num;
    else
        resData[1] = app_bt_is_device_profile_connected_num();

    sys_memcpy_swap(&resData[2],bt_global_addr,6);

    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, 8);
    TRACE(0,"[UIAPP]%s", __func__);
    DUMP8("%02X ", resData, 8);
}

void app_tota_tws_status_to_app(uint8_t sta)
{
#ifndef FREEMAN_ENABLED_STERO
    uint8_t resData[3]={0};
    resData[0] = APP_TOTA_TWS_STATUS;
    if(sta != 0xFF)
        resData[1] = sta;
    else
        resData[1] = app_tws_is_connected();

    if(app_ibrt_if_is_left_side())
    {
        if(IBRT_SLAVE != app_ibrt_if_get_ui_role())
            resData[2] = 1;
    }
    else
    {
        if(IBRT_SLAVE != app_ibrt_if_get_ui_role())
            resData[2] = 2;
    }
    TRACE(4,"[UIAPP]%s, %02X %02X %02X", __func__, resData[0], resData[1], resData[2]);
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, 3);
#endif
}

void app_tota_general_button_event_info_set(uint8_t* ptrParam, uint16_t paramLen)
{
    uint8_t resData[5];
    uint8_t resLen;
    TRACE(1,"[UIAPP], button ptrParam = %d, %d, %d, %d", ptrParam[0], ptrParam[1], ptrParam[2], ptrParam[3]);
   if(BUTTON_SETTING_LEFT_EARPHONE_CMD == ptrParam[1])
   {
        if(!app_tws_is_connected()&&!app_ibrt_if_is_left_side())
        {
            resData[0] = ptrParam[0];
            resData[1] = ptrParam[1];
            resData[2] = ptrParam[2];
            resData[3] = ptrParam[3];
            resData[4] = 0;
            resLen = 0x05;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            return ;
        }
       if(BUTTON_SETTING_CLICK_CMD == ptrParam[2])
       {
           button_event_info_left[0].status.code = BUTTON_MAP_KEY_TYPE;//if you have button parameters,set them here
           button_event_info_left[0].status.event = APP_KEY_EVENT_CLICK;//8,if you have button event parameters,set them here
           button_event_info_left[0].lr = ptrParam[1];
           button_event_info_left[0].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_DOUBLE_CLICK_CMD == ptrParam[2])
       {
           button_event_info_left[1].status.code = BUTTON_MAP_KEY_TYPE;//if you have button parameters,set them here
           button_event_info_left[1].status.event = APP_KEY_EVENT_DOUBLECLICK;//if you have button event parameters,set them here
           button_event_info_left[1].lr = ptrParam[1];
           button_event_info_left[1].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_TRIPLE_CLICK_CMD == ptrParam[2])
       {
           button_event_info_left[2].status.code = BUTTON_MAP_KEY_TYPE;
           button_event_info_left[2].status.event = APP_KEY_EVENT_TRIPLECLICK;
           button_event_info_left[2].lr = ptrParam[1];
           button_event_info_left[2].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_LONG_PRESS_CMD == ptrParam[2])
       {
           button_event_info_left[3].status.code = BUTTON_MAP_KEY_TYPE;
           button_event_info_left[3].status.event = APP_KEY_EVENT_INITLONGPRESS;
           button_event_info_left[3].lr = ptrParam[1];
           button_event_info_left[3].button_set_event = ptrParam[3];
       }
        //write to flash
        TRACE(1,"write flash, %d",  sizeof(button_event_info_left));
        memcpy(p_nvrecord_env->buttonInfo_left, button_event_info_left, sizeof(button_event_info_left));
        //DUMP8("%0x2", &(p_nvrecord_env->buttonInfo_left),  sizeof(button_event_info_left));
        nv_record_env_set(p_nvrecord_env);
        //nv_record_flash_flush();
   }
   else
   {
        if(!app_tws_is_connected()&&app_ibrt_if_is_left_side())
        {
            resData[0] = ptrParam[0];
            resData[1] = ptrParam[1];
            resData[2] = ptrParam[2];
            resData[3] = ptrParam[3];
            resData[4] = 0;
            resLen = 0x05;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            return ;
        }
       if(BUTTON_SETTING_CLICK_CMD == ptrParam[2])
       {
           button_event_info_right[0].status.code = BUTTON_MAP_KEY_TYPE;//if you have button parameters,set them here
           button_event_info_right[0].status.event = APP_KEY_EVENT_CLICK;//8,if you have button event parameters,set them here
           button_event_info_right[0].lr = ptrParam[1];
           button_event_info_right[0].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_DOUBLE_CLICK_CMD == ptrParam[2])
       {
           button_event_info_right[1].status.code = BUTTON_MAP_KEY_TYPE;//if you have button parameters,set them here
           button_event_info_right[1].status.event = APP_KEY_EVENT_DOUBLECLICK;//if you have button event parameters,set them here
           button_event_info_right[1].lr = ptrParam[1];
           button_event_info_right[1].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_TRIPLE_CLICK_CMD == ptrParam[2])
       {
           button_event_info_right[2].status.code = BUTTON_MAP_KEY_TYPE;
           button_event_info_right[2].status.event = APP_KEY_EVENT_TRIPLECLICK;
           button_event_info_right[2].lr = ptrParam[1];
           button_event_info_right[2].button_set_event = ptrParam[3];
       }
       else if(BUTTON_SETTING_LONG_PRESS_CMD == ptrParam[2])
       {
           button_event_info_right[3].status.code = BUTTON_MAP_KEY_TYPE;
           button_event_info_right[3].status.event = APP_KEY_EVENT_INITLONGPRESS;
           button_event_info_right[3].lr = ptrParam[1];
           button_event_info_right[3].button_set_event = ptrParam[3];
       }

    
        //write to flash
        TRACE(1,"write flash, %d",  sizeof(button_event_info_right));
        memcpy(p_nvrecord_env->buttonInfo_right, button_event_info_right, sizeof(button_event_info_right));
       // DUMP8("%0x2", &(p_nvrecord_env->buttonInfo_right),  sizeof(button_event_info_right));
        nv_record_env_set(p_nvrecord_env);
        //nv_record_flash_flush();
   }
    //if(app_ibrt_if_is_ui_master())
    if(TWS_UI_SLAVE != app_ibrt_if_get_ui_role())
    {
        resData[0] = ptrParam[0];
        resData[1] = ptrParam[1];
        resData[2] = ptrParam[2];
        resData[3] = ptrParam[3];
        resData[4] = 1;
        resLen = 0x05;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
    }
    return ;
}

static void app_tota_general_cmd_deal_with_custom_cmd(uint8_t* ptrParam, uint32_t paramLen)
{
    uint8_t custom_cmd_type = ptrParam[0];
    uint8_t resData[20]={0};
    uint32_t resLen=1;

    uint8_t device_id = app_bt_audio_get_device_for_user_action();

    TRACE(0,"[UIAPP]%s", __func__);
    TRACE(0,"[UIAPP]custom_cmd_type =%d", custom_cmd_type);
    DUMP8("%02x ", ptrParam, paramLen);
    //TRACE(1,"tota_get_connect_path is %d", tota_get_connect_path());

#ifndef FREEMAN_ENABLED_STERO
    if(app_ibrt_if_is_ui_slave()) //compatible SPP&BLE
    {
        TOTA_LOG_DBG(0,"[%s], slave return, cmd: %d;", __func__, custom_cmd_type);
        return ;
    }
#endif
    if(APP_TOTA_BUTTON_SETTINGS_CONTROL_CMD == custom_cmd_type)
    {
        //for the ble connection, it needs to be send to the slave ear
        //if(APP_TOTA_VIA_NOTIFICATION == tota_get_connect_path())
        if(app_tws_is_connected())
        {
            tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_TOTA_BUTTON_SETTINGS_CONTROL, ptrParam, (uint16_t)paramLen); ///>send to slave
        }
        app_tota_general_button_event_info_set(ptrParam, (uint16_t)paramLen);

        return;
    }
    else if( APP_TOTA_FACTORY_RESET_CMD == custom_cmd_type)
    {
        TRACE(1,"[UIAPP]custom: factory reset  connect path %d",tota_get_connect_path());
        static bool rst_flag = 1;
        if(rst_flag)
        {
            rst_flag = 0;
            if(app_tws_is_connected())
                tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_TOTA_FACTORY_RESET, NULL, 0); ///>send to slave
                
            app_tota_general_button_event_reset();
            nv_record_rebuild(NV_REBUILD_SDK_ONLY);
            bes_ble_gap_disconnect_all();     
            nv_record_env_set(p_nvrecord_env);
        }
        return ;
    }
    else if(APP_TOTA_MUSIC_PLAY_SETTINGS_CMD == custom_cmd_type)
    {
        if(MUSIC_PLAY_SETTINGS_PLAY_CMD == ptrParam[1])
        {
            app_ibrt_if_a2dp_send_play(device_id);
        }
        else if(MUSIC_PLAY_SETTINGS_PAUSE_CMD == ptrParam[1])
        {
            app_ibrt_if_a2dp_send_pause(device_id);
        }
        else if(MUSIC_PLAY_SETTINGS_NEXT_CMD == ptrParam[1])
        {
            app_ibrt_if_a2dp_send_forward(device_id);
        }
        else if(MUSIC_PLAY_SETTINGS_PRE_CMD == ptrParam[1])
        {
            app_ibrt_if_a2dp_send_backward(device_id);
        }
        else
        {
            TRACE(0,"[UIAPP]error command");
        }
        return ;
    }
    else if(APP_TOTA_BATTERY_LEVEL_CMD == custom_cmd_type)
    {
        if(ptrParam[1] == INEAR_DETECT_RIGHT_EAR_CMD)
        {
            if(app_ibrt_if_is_right_side())
            {
                uint8_t battery_levels = (app_battery_current_level()+1) * 10;
                if(battery_levels >= 100)
                    battery_levels = 100;
                resData[0] = APP_TOTA_BATTERY_LEVEL_CMD;
                resData[1] = INEAR_DETECT_RIGHT_EAR_CMD;
                resData[2] = battery_levels;
                resLen = 0x03;
                app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            }
            else
            {
                if(app_tws_is_connected())
                {
                   tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_BATTERY_LEVEL_CMD, &ptrParam[1], 1);
                }
            }
        }
        else if(ptrParam[1] == INEAR_DETECT_LEFT_EAR_CMD)
        {
            if(app_ibrt_if_is_left_side())
            {
                uint8_t battery_levels = (app_battery_current_level()+1) * 10;
                if(battery_levels >= 100)
                    battery_levels = 100;
                resData[0] = APP_TOTA_BATTERY_LEVEL_CMD;
                resData[1] = INEAR_DETECT_LEFT_EAR_CMD;
                resData[2] = battery_levels;
                resLen = 0x03;
                app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            }
            else
            {
                if(app_tws_is_connected())
                {
                   tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_BATTERY_LEVEL_CMD, &ptrParam[1], 1);
                }
            }
        }
        else if(ptrParam[1] == INEAR_DETECT_BOX_CMD)
        {
            uint8_t battery_levels = 0;
#if !defined(FREEMAN_ENABLED_STERO)
            battery_levels = current_box_cell_battery;
#endif
            if(battery_levels >= 100)
                battery_levels = 100;
            resData[0] = APP_TOTA_BATTERY_LEVEL_CMD;
            resData[1] = INEAR_DETECT_BOX_CMD;
            resData[2] = battery_levels;
            resLen = 0x03;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        }
        return;
    }
#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
    else if(APP_TOTA_EARBUD_FIT_TEST_CMD == custom_cmd_type)
    {
        TRACE(1,"[UIAPP]earbud fit test, path is %d", tota_get_connect_path());
        if(ptrParam[1] == EARBUD_DETECK_ON_CMD)
        {
            if(a2dp_is_music_ongoing())
            {
                start_leak_detect_flag = true;
                app_ibrt_if_a2dp_send_pause(device_id);
            }
            else
            {
               app_tota_start_leak_detect();
            }
        }
        return ;
    }
#endif
    else if(APP_TOAT_EQ_CMD == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]EQ set");
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
        ptrParam[2] = ptrParam[1];             
        ptrParam[1] = p_nvrecord_env->eq_onoff;
        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3); ///>send to slave

        algo_send_request(ALGO_ID_EQ, p_nvrecord_env->eq_onoff, ptrParam[2], 0, 0, 0);
#endif
    }
    else if(APP_TOTA_INEAR_DETECT_CMD == custom_cmd_type)
    {
        resData[0] = APP_TOTA_INEAR_DETECT_CMD;
        if(INEAR_DETECT_STATUS_CMD == ptrParam[1])
        {
            TRACE(0,"[UIAPP]INEAR_DETECT_STATUS_CMD");
            resData[1] = INEAR_DETECT_STATUS_CMD;
            if(paramLen == 2) //app get sta
            {
                if(p_nvrecord_env->inear_left_onoff != 1)
                    p_nvrecord_env->inear_left_onoff = 0;
                if(p_nvrecord_env->left_status != 1)
                    p_nvrecord_env->left_status = 0;
                if(p_nvrecord_env->inear_right_onoff != 1)
                    p_nvrecord_env->inear_right_onoff = 0;
                if(p_nvrecord_env->right_status != 1)
                    p_nvrecord_env->right_status = 0;
                resData[2] = p_nvrecord_env->inear_left_onoff;
                resData[3] = app_inear_get_left_status();//p_nvrecord_env->left_status;
                resData[4] = p_nvrecord_env->inear_right_onoff;
                resData[5] = app_inear_get_right_status();//p_nvrecord_env->right_status;
                resLen = 0x06;
                app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD,TOTA_NO_ERROR,resData,resLen);
            }
            else if(paramLen == 3) //app set sta
            {
                if(INEAR_DETECT_ON_CMD == ptrParam[2]) //left and right on
                {
                    TRACE(0,"[UIAPP]wear detect all on"); 
                    p_nvrecord_env->inear_left_onoff = INEAR_DETECT_ON_CMD; 
                    p_nvrecord_env->left_status = app_inear_get_left_status();
                    p_nvrecord_env->inear_right_onoff = INEAR_DETECT_ON_CMD;
                    p_nvrecord_env->right_status = app_inear_get_right_status();
                }
                else                                  //left and right off               
                {
                    TRACE(0,"[UIAPP]wear detect all off");
                    p_nvrecord_env->inear_left_onoff = 0x00;
                    p_nvrecord_env->left_status = 0x00;

                    p_nvrecord_env->inear_right_onoff = 0x00;
                    p_nvrecord_env->right_status = 0x00;
                }
                nv_record_env_set(p_nvrecord_env);

                if(app_tws_is_connected())
                {
                    ptrParam[0] = 0xFF;
                    ptrParam[1] = p_nvrecord_env->inear_left_onoff;
                    tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_INEAR_ONOFF_CMD, &ptrParam[0], 2);
                }
                resData[2] = p_nvrecord_env->inear_left_onoff;
                resData[3] = p_nvrecord_env->left_status;
                resData[4] = p_nvrecord_env->inear_right_onoff;
                resData[5] = p_nvrecord_env->right_status;
                resLen = 0x06;
                app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            }
        }
        else if(INEAR_DETECT_RIGHT_EAR_CMD == ptrParam[1])     //right ear is master
        { 
            TRACE(0,"[UIAPP]INEAR_DETECT_RIGHT_EAR_CMD");
            if(app_ibrt_if_is_left_side())
            {
                if(app_tws_is_connected())
                {
                    tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_INEAR_ONOFF_CMD, &ptrParam[2], 1);
                }
                else
                {
                    resData[0] = APP_TOTA_INEAR_DETECT_CMD;
                    resData[1] = INEAR_DETECT_RIGHT_EAR_CMD;
                    resData[2] = 0;
                    resData[3] = 0;
                    resLen = 0x04;
                    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
                    return ;
                }
            }

            if(INEAR_DETECT_ON_CMD == ptrParam[2])
            {
                p_nvrecord_env->inear_right_onoff = INEAR_DETECT_ON_CMD; 
                nv_record_env_set(p_nvrecord_env);
                //nv_record_flash_flush();
            }
            else
            {
                p_nvrecord_env->inear_right_onoff = INEAR_DETECT_OFF_CMD;
                p_nvrecord_env->right_status = 0x00;
                nv_record_env_set(p_nvrecord_env);
                //nv_record_flash_flush();
            }

            resData[0] = APP_TOTA_INEAR_DETECT_CMD;
            resData[1] = INEAR_DETECT_RIGHT_EAR_CMD;
            resData[2] = p_nvrecord_env->inear_right_onoff;
            resData[3] = app_inear_get_right_status();//p_nvrecord_env->right_status;
            resLen = 0x04;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        }
        else if(INEAR_DETECT_LEFT_EAR_CMD == ptrParam[1])       //left is slave
        {
            TRACE(0,"[UIAPP]INEAR_DETECT_LEFT_EAR_CMD");
            if(app_ibrt_if_is_right_side())
            {
                if(app_tws_is_connected())
                {
                    tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_INEAR_ONOFF_CMD, &ptrParam[2], 1);
                }
                else
                {
                    resData[0] = APP_TOTA_INEAR_DETECT_CMD;
                    resData[1] = INEAR_DETECT_LEFT_EAR_CMD;
                    resData[2] = 0;
                    resData[3] = 0;
                    resLen = 0x04;
                    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
                    return ;
                }
            }

            if(INEAR_DETECT_ON_CMD == ptrParam[2])
            {
                p_nvrecord_env->inear_left_onoff = INEAR_DETECT_ON_CMD; 
                nv_record_env_set(p_nvrecord_env);
                //nv_record_flash_flush();
            }
            else
            {
                p_nvrecord_env->inear_left_onoff = INEAR_DETECT_OFF_CMD;
                p_nvrecord_env->left_status = 0x00;
                nv_record_env_set(p_nvrecord_env);
                //nv_record_flash_flush();
            }

            resData[0] = APP_TOTA_INEAR_DETECT_CMD;
            resData[1] = INEAR_DETECT_LEFT_EAR_CMD;
            resData[2] = p_nvrecord_env->inear_left_onoff;
            resData[3] = app_inear_get_left_status();//p_nvrecord_env->left_status;
            resLen = 0x04;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        }
        else if(INEAR_DETECT_PROMPT == ptrParam[1])
        {
            TRACE(0,"[UIAPP]INEAR_DETECT_PROMPT");
#if defined(WEAR_DETECT_PROMPT_EN)
            resData[1] = INEAR_DETECT_PROMPT;
            if(paramLen == 3) //app set sta
            {
                p_nvrecord_env->wear_prompt_onoff = ptrParam[2]; 
                nv_record_env_set(p_nvrecord_env);
                // nv_record_flash_flush();
            }
            resData[2] = p_nvrecord_env->wear_prompt_onoff;
            resLen = 3;
            app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
            if(app_tws_is_connected())
            {
                resData[0] = 0xFF;
                resData[1] = 0xFF;
                resData[2] = p_nvrecord_env->wear_prompt_onoff;
                tws_ctrl_send_cmd(APP_TWS_CMD_SEND_TOTA_INEAR_ONOFF_CMD, resData, 3);
            }
#endif
        }
        return ;
    }
    else if(APP_TOTA_GET_SPP_STATUS_CMD == custom_cmd_type)
    {
        uint8_t ret = 0;
        // resData[0] = custom_cmd_type;
        if(tota_get_connect_path() == 0)
            ret = 1;
        else
            ret = 2;
        resData[0] = custom_cmd_type;
        resData[1] = ret;
        resLen = 2;
        TRACE(0,"[UIAPP]APP_TOTA_GET_SPP_STATUS : %d",ret);
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ;
    }
    else if(APP_TOTA_GET_BT_ADDR_CMD == custom_cmd_type)
    {
        resData[0] = custom_cmd_type;
        resData[1] = app_bt_is_device_profile_connected_num();
        sys_memcpy_swap(&resData[2],bt_global_addr,6);
        resLen = 8;
        TRACE(0,"[UIAPP]APP_TOTA_GET_APP_ADDR ");
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ;
    }
    else if(APP_TOTA_GET_CURRENT_BUTTON_STATE == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]APP_TOTA_GET_CURRENT_BUTTON_STATE ");
        resData[0] = custom_cmd_type;
        resData[1] = BUTTON_SETTING_LEFT_CLICK_CMD;
        resData[2] = p_nvrecord_env->buttonInfo_left[0].button_set_event;
        resData[3] = BUTTON_SETTING_RIGHT_CLICK_CMD;
        resData[4] = p_nvrecord_env->buttonInfo_right[0].button_set_event;
        resData[5] = BUTTON_SETTING_LEFT_DOUBLE_CMD;
        resData[6] = p_nvrecord_env->buttonInfo_left[1].button_set_event;
        resData[7] = BUTTON_SETTING_RIGHT_DOUBLE_CMD;
        resData[8] = p_nvrecord_env->buttonInfo_right[1].button_set_event;
        resData[9] = BUTTON_SETTING_LEFT_TRIPLE_CMD;
        resData[10] = p_nvrecord_env->buttonInfo_left[2].button_set_event;
        resData[11] = BUTTON_SETTING_RIGHT_TRIPLE_CMD;
        resData[12] = p_nvrecord_env->buttonInfo_right[2].button_set_event;
        resData[13] = BUTTON_SETTING_LEFT_LONG_PRESS_CMD;
        resData[14] = p_nvrecord_env->buttonInfo_left[3].button_set_event;
        resData[15] = BUTTON_SETTING_RIGHT_LONG_PRESS_CMD;
        resData[16] = p_nvrecord_env->buttonInfo_right[3].button_set_event;
        
        resLen = 17;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ;

    }
    else if(APP_TOTA_GET_PRODUCT_MODEL == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]GET_PRODUCT_MODEL");
        const char* model_id = PRODUCT_MODEL;

        uint8_t  model_id_len = strlen(model_id);
        resLen = model_id_len>57? 57:model_id_len;
        resData[0] = custom_cmd_type;
        resData[1] = (uint8_t) resLen;
        resData[2] = (uint8_t) resLen>>8;
        memcpy(&resData[3], model_id, resLen);
        resLen += 3;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ; 
    }
    else if(APP_TOTA_REGULATING_ANC == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]APP_TOTA_REGULATING_ANC");
        uint8_t ANC_STATUS = 0;
        if(APP_TOTA_GET_ANC_STATUS == ptrParam[1])
        {
            TRACE(0,"[UIAPP]GET_ANC_STATUS");
#if defined(ANC_APP)
            // if(app_anc_get_curr_mode() == APP_ANC_MODE_OFF)
            //     ANC_STATUS = 4;
            // else
            //     ANC_STATUS = app_anc_get_curr_mode();
            app_tota_anc_send_to_app(app_anc_get_curr_mode());
#endif
        }
        else if(APP_TOTA_SET_ANC_STATUS == ptrParam[1])
        {
            TRACE(0,"[UIAPP]SET_ANC_STATUS");
            ANC_STATUS = ptrParam[2];
#ifdef ALGO_INFO_SYNC_EN
            algo_send_request(ALGO_ID_ANC, ANC_STATUS, 0, 0, 0, 1);
#endif
        }
        resData[0] = custom_cmd_type;
        resData[1] = ANC_STATUS;
        resLen = 2;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ;
    }
    else if(APP_TOTA_REGULATING_EQ == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]REGULATING_EQ");
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
        uint8_t EQ_STATUS = 0;
        uint8_t EQ_MODE = 0;
#endif
        if(APP_TOTA_GET_EQ_ONOFF_STATUS == ptrParam[1])
        {
            TRACE(0,"[UIAPP]GET_EQ_STATUS");
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
            EQ_STATUS = p_nvrecord_env->eq_onoff;
#endif
        }
        else if(APP_TOTA_SET_EQ_ONOFF_STATUS == ptrParam[1])
        {
            TRACE(0,"[UIAPP]SET_EQ_STATUS");
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
            EQ_STATUS = ptrParam[2];
            if(p_nvrecord_env->eq_mode == 0xFF)
                p_nvrecord_env->eq_mode = 0;
            algo_send_request(ALGO_ID_EQ, EQ_STATUS, p_nvrecord_env->eq_mode, 0, 0, 0);
#if !defined(FREEMAN_ENABLED_STERO)
            ptrParam[1] = EQ_STATUS;
            ptrParam[2] = p_nvrecord_env->eq_mode;
            tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3);
#endif
#endif
        }
        else if(APP_TOTA_SET_EQ_PARAMETER == ptrParam[1])
        {
            TRACE(0,"[UIAPP]SET_EQ_MODE");
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
            EQ_MODE = ptrParam[2];
            algo_send_request(ALGO_ID_EQ, p_nvrecord_env->eq_onoff, EQ_MODE, 0, 0, 0);
#if !defined(FREEMAN_ENABLED_STERO)
            ptrParam[1] = p_nvrecord_env->eq_onoff;
            tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_EQ, ptrParam, 3);
#endif
#endif
        }
#if defined(ALGO_INFO_SYNC_EN) && defined(EQ_SET_CUSTOMER_EN)
        resData[0] = custom_cmd_type;
        resData[1] = EQ_STATUS?EQ_STATUS:p_nvrecord_env->eq_onoff;
        resData[2] = EQ_MODE?EQ_MODE:p_nvrecord_env->eq_mode;
#endif
        resLen = 3;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, resData, resLen);
        return ;
    }
    else if(APP_TOTA_TWS_STATUS == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]%s, APP_TOTA_TWS_STATUS", __func__);
        resData[0] = custom_cmd_type;
        resData[1] = app_tws_is_connected();
        resData[2] = app_ibrt_if_is_left_side()?1:2;
        resLen = 3;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD,TOTA_NO_ERROR,resData,resLen);
        return ;
    }
#ifdef APP_MULTIPOINT_ONOFF_EN
    else if(APP_TOTA_MULTIPOINT == custom_cmd_type)
    {
        TRACE(0,"[UIAPP]%s, APP_TOTA_MULTIPOINT", __func__);
        resData[0] = custom_cmd_type;
        // if(paramLen == 2) //get
        if(paramLen == 3)
        {
            if(0x00 == ptrParam[1]) //set
            {
                if(0x01 == ptrParam[2])
                    p_nvrecord_env->muiltipoint_onoff = 1;
                else
                    p_nvrecord_env->muiltipoint_onoff = 0;
                nv_record_env_set(p_nvrecord_env);
                algo_send_request(ALGO_ID_MULTIPOINT, p_nvrecord_env->muiltipoint_onoff, 0, 0, 0, 0);
            }
        }

        resData[1] = 0x01;
        resData[2] = p_nvrecord_env->muiltipoint_onoff;
        resLen = 3;
        app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD,TOTA_NO_ERROR,resData,resLen);
        return ;
    }  
#endif
    else
    {
        TRACE(0,"[UIAPP]error custom cmd type");
    }
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD,TOTA_NO_ERROR,resData,resLen);
}
    
#if defined(VOICE_ASSIST_CUSTOM_LEAK_DETECT)
void app_tota_sync_leak_deteck_handle(uint8_t *p_buff, uint8_t length)
{
    app_sysfreq_req(APP_SYSFREQ_USER_CUSTOM_DECTET, APP_SYSFREQ_208M);
    app_voice_assist_custom_leak_detect_open();

    app_sysfreq_req(APP_SYSFREQ_USER_CUSTOM_DECTET, APP_SYSFREQ_32K);
}
extern "C" void app_tota_leak_detect_result();
void app_tota_leak_detect_result()
{

    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    uint8_t leak_detect_status= app_voice_assitant_get_status();
    
    if(IBRT_SLAVE == app_ibrt_if_get_ui_role() && p_ibrt_ctrl->init_done)
    {
        TRACE(0,"slave send status to master");
        tws_ctrl_send_cmd(APP_TWS_CMD_TOTA_SEND_LEAK_DETECT_STATUS, &leak_detect_status, 1);
    }
}

void app_ibrt_send_leak_deteck_status_handle()
{
    uint8_t leak_resData[60]={0};
    uint32_t leak_resLen=1;
    uint8_t leak_detect_status= app_voice_assitant_get_status();

    leak_resData[0] = APP_TOTA_EARBUD_FIT_TEST_CMD;
    leak_resData[1] = leak_detect_status;
    leak_resData[2] = app_ibrt_tota_get_leak_detect_status();
    leak_resLen = 0x03;
    app_tota_send_rsp(OP_TOTA_SET_CUSTOMER_CMD, TOTA_NO_ERROR, leak_resData, leak_resLen);
}
#endif

void app_tota_sync_audio_eq_handle(uint8_t *p_buff, uint8_t length)
{
    bt_audio_updata_eq(*p_buff);
}
#endif
/*
**  handle general cmd
*/
static void __tota_general_cmd_handle(APP_TOTA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);

/*------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------*/

static void __tota_general_cmd_handle(APP_TOTA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen)
{
    TOTA_LOG_DBG(3,"[%s]: opCode:0x%x, paramLen:%d", __func__, funcCode, paramLen);
    DUMP8("%02x ", ptrParam, paramLen);
    uint8_t resData[60]={0};
    uint32_t resLen=1;
    uint8_t volume_level;
#ifdef CAPSENSOR_TOTA_DATA
    struct BES_BT_DEVICE_T *curr_device = NULL;
    ibrt_mobile_info_t *p_mobile_info = NULL;
    bt_bdaddr_t *mobile_addr = NULL;
#endif

    switch (funcCode)
    {
        case OP_TOTA_GENERAL_INFO_CMD:
            __get_general_info();
            app_tota_send_data(funcCode, (uint8_t*)&general_info, sizeof(general_info_t));
            return ;
        case OP_TOTA_EXCHANGE_MTU_CMD:
            tota_set_trans_MTU(*(uint16_t *)ptrParam);
            #if(BLE_APP_OTA_OVER_TOTA)
                ota_control_update_MTU(*(uint16_t *)ptrParam - TOTA_PACKET_VERIFY_SIZE);
            #endif
           break;
        case OP_TOTA_GET_RSSI_CMD:
#ifdef IBRT
            app_ibrt_rssi_get_stutter(resData, &resLen);
            app_tota_send_data(funcCode, resData, resLen);
#endif
            return;
        case OP_TOTA_VOLUME_PLUS_CMD:
            app_bt_volumeup();
            volume_level = app_bt_stream_local_volume_get();
            // resData[0] = volume_level;
            TRACE(1,"volume = %d",volume_level);
            break;
        case OP_TOTA_VOLUME_DEC_CMD:
            app_bt_volumedown();
            volume_level = app_bt_stream_local_volume_get();
            // resData[0] = volume_level;
            // resLen = 1;
            TRACE(1,"volume = %d",volume_level);
            break;
        case OP_TOTA_VOLUME_SET_CMD:
            //uint8_t scolevel = ptrParam[0];
            //uint8_t a2dplevel = ptrParam[1];
            app_bt_set_volume(APP_BT_STREAM_HFP_PCM,ptrParam[0]);
            app_bt_set_volume(APP_BT_STREAM_A2DP_SBC,ptrParam[1]);
            bes_bt_hfp_report_speak_gain();
            bes_bt_a2dp_report_speak_gain();
            break;
        case OP_TOTA_VOLUME_GET_CMD:
            resData[0] = app_bt_stream_hfpvolume_get();
            resData[1] = app_bt_stream_a2dpvolume_get();
            resLen = 2;
            app_tota_send_data(funcCode, resData, resLen);
            return;
        case OP_TOTA_RAW_DATA_SET_CMD:
            // app_ibrt_debug_parse(ptrParam, paramLen);
            break;
        case OP_TOTA_GET_DUMP_INFO_CMD:
        {
            TOTA_DUMP_INFO_STRUCT_T dump_info;
            general_get_flash_dump_info(&dump_info);
            app_tota_send_rsp(funcCode,TOTA_NO_ERROR,(uint8_t*)&dump_info, sizeof(TOTA_DUMP_INFO_STRUCT_T));
            return ;
        }
        case OP_TOTA_SET_CUSTOMER_CMD:
#ifdef CUSTOMER_APP_BOAT
            app_tota_general_cmd_deal_with_custom_cmd(ptrParam, paramLen);
            TRACE(0, "[%s] OP_TOTA_SET_CUSTOMER_CMD ...", __func__);
            return ;
#endif
        case OP_TOTA_GET_CUSTOMER_CMD:
            TRACE(0, "[%s] OP_TOTA_GET_CUSTOMER_CMD ...", __func__);
            break;
#ifdef CUSTOMER_APP_BOAT
        case OP_TOTA_CRC_CHECK_CMD:
        {
            APP_TOTA_CMD_RET_STATUS_E status = TOTA_NO_ERROR;
            TRACE(1, "[%s] OP_TOTA_CRC_CHECK_CMD ...", __func__);

            if(*(int*)ptrParam == TOTA_CRC_CHECK){

                TOTA_CRC_CHECK_STRUCT_T *pdata = (TOTA_CRC_CHECK_STRUCT_T *)resData;

                status = tota_get_sanity_crc(&pdata->crc);

                status = tota_get_buidl_data(pdata->build_date);
#ifdef FIRMWARE_REV
                system_get_info(&(pdata->version[0]),&(pdata->version[1]),&(pdata->version[2]),&(pdata->version[3]));
#endif
                TRACE(0, "|--------------------------------|");
                TRACE(1, "CRC: 0x%x", pdata->crc);
                TRACE(1, "Version: %d.%d.%d.%d", pdata->version[0],pdata->version[1],pdata->version[2],pdata->version[3]);
                TRACE(1, "Build Data: %s", pdata->build_date);
                TRACE(0, "|--------------------------------|");
            }else{
                status =TOTA_INVALID_DATA_PACKET;
            }

            resLen = sizeof(TOTA_CRC_CHECK_STRUCT_T);
            app_tota_send_rsp(funcCode, status, resData, resLen);
            return;
        }
#endif
        default:
            // TRACE(1,"wrong cmd 0x%x",funcCode);
            // resData[0] = -1;
            return;
    }
    app_tota_send_rsp(funcCode,TOTA_NO_ERROR,resData,resLen);
}

/* get general info */
static void __get_general_info()
{
    /* get bt-ble name */
    uint8_t* factory_name_ptr =factory_section_get_bt_name();
    if ( factory_name_ptr != NULL )
    {
        uint16_t valid_len = strlen((char*)factory_name_ptr) > BT_BLE_LOCAL_NAME_LEN? BT_BLE_LOCAL_NAME_LEN:strlen((char*)factory_name_ptr);
        memcpy(general_info.btName,factory_name_ptr,valid_len);
    }

    factory_name_ptr =factory_section_get_ble_name();
    if ( factory_name_ptr != NULL )
    {
        uint16_t valid_len = strlen((char*)factory_name_ptr) > BT_BLE_LOCAL_NAME_LEN? BT_BLE_LOCAL_NAME_LEN:strlen((char*)factory_name_ptr);
        memcpy(general_info.bleName,factory_name_ptr,valid_len);
    }

    /* get bt-ble peer addr */
//    ibrt_config_t addrInfo;
//    app_ibrt_ui_test_config_load(&addrInfo);
//    general_info.ibrtRole = addrInfo.nv_role;
//    memcpy(general_info.btLocalAddr.address, addrInfo.local_addr.address, 6);
//    memcpy(general_info.btPeerAddr.address, addrInfo.peer_addr.address, 6);

    #ifdef BLE
    memcpy(general_info.bleLocalAddr.address, bt_get_ble_local_address(), 6);
    memcpy(general_info.blePeerAddr.address, nv_record_tws_get_peer_ble_addr(), 6);
    #endif

    /* get crystal info */
    general_info.crystal_freq = hal_cmu_get_crystal_freq();

    /* factory_section_xtal_fcap_get */
    factory_section_xtal_fcap_get(&general_info.xtal_fcap);

    /* get battery info (volt level)*/
    app_battery_get_info(&general_info.battery_volt,&general_info.battery_level,&general_info.battery_status);

    /* get firmware version */
#ifdef FIRMWARE_REV
    system_get_info(&general_info.fw_version[0],&general_info.fw_version[1],&general_info.fw_version[2],&general_info.fw_version[3]);
    TRACE(4,"firmware version = %d.%d.%d.%d",general_info.fw_version[0],general_info.fw_version[1],general_info.fw_version[2],general_info.fw_version[3]);
#endif

#ifdef IBRT
    /* get ear location info */
    if ( app_ibrt_if_is_right_side() )      general_info.ear_location = EAR_SIDE_RIGHT;
    else if ( app_ibrt_if_is_left_side() )  general_info.ear_location = EAR_SIDE_LEFT;
    else                                general_info.ear_location = EAR_SIDE_UNKNOWN;
#endif

    general_info.rssi[0] = app_tota_get_rssi_value();
    general_info.rssi_len = 1;
}

#ifdef CUSTOMER_APP_BOAT
static int32_t find_key_word(uint8_t* targetArray, uint32_t targetArrayLen,
    uint8_t* keyWordArray,
    uint32_t keyWordArrayLen)
{
    if ((keyWordArrayLen > 0) && (targetArrayLen >= keyWordArrayLen))
    {
        uint32_t index = 0, targetIndex = 0;
        for (targetIndex = 0;targetIndex < targetArrayLen;targetIndex++)
        {
            for (index = 0;index < keyWordArrayLen;index++)
            {
                if (targetArray[targetIndex + index] != keyWordArray[index])
                {
                    break;
                }
            }

            if (index == keyWordArrayLen)
            {
                return targetIndex;
            }
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

static uint8_t asciiToHex(uint8_t asciiCode)
{
    if ((asciiCode >= '0') && (asciiCode <= '9'))
    {
        return asciiCode - '0';
    }
    else if ((asciiCode >= 'a') && (asciiCode <= 'f'))
    {
        return asciiCode - 'a' + 10;
    }
    else if ((asciiCode >= 'A') && (asciiCode <= 'F'))
    {
        return asciiCode - 'A' + 10;
    }
    else
    {
        return 0xff;
    }
}

static APP_TOTA_CMD_RET_STATUS_E tota_get_sanity_crc(uint32_t *sanityCrc32) 
{
    if(NULL == sanityCrc32){
        return TOTA_CMD_HANDLING_FAILED;
    }

    int32_t found = find_key_word((uint8_t*)&sys_build_info,
        LEN_OF_IMAGE_TAIL_TO_FINDKEY_WORD,
        (uint8_t*)image_info_sanity_crc_key_word,
        strlen(image_info_sanity_crc_key_word));
    if (-1 == found){
        return TOTA_CMD_HANDLING_FAILED;
    }

    uint8_t* crcString = (uint8_t*)&sys_build_info+found+strlen(image_info_sanity_crc_key_word);

    for (uint8_t index = 0;index < 4;index++)
    {
        *sanityCrc32 |= (asciiToHex(crcString[2*index]) << (8*index+4)) + (asciiToHex(crcString[2*index+1]) << (8*index));
    }

    TRACE(1,"sanityCrc32 is 0x%x", *sanityCrc32);

    return TOTA_NO_ERROR;
}

static APP_TOTA_CMD_RET_STATUS_E tota_get_buidl_data(uint8_t *buildData) 
{
    if(NULL == buildData){
        return TOTA_CMD_HANDLING_FAILED;
    }
    int32_t found = find_key_word((uint8_t*)&sys_build_info,
        LEN_OF_IMAGE_TAIL_TO_FINDKEY_WORD,
        (uint8_t*)image_info_build_data,
        strlen(image_info_build_data));
    if (-1 == found){
        return TOTA_CMD_HANDLING_FAILED;
    }

    memcpy(buildData, (uint8_t*)&sys_build_info+found+strlen(image_info_build_data), 20);

    TRACE(1,"buildData is 0x%s", buildData);

    return TOTA_NO_ERROR;
}
#endif

/* general command */
TOTA_COMMAND_TO_ADD(OP_TOTA_GENERAL_INFO_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_VOLUME_PLUS_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_VOLUME_DEC_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_VOLUME_SET_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_VOLUME_GET_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_RAW_DATA_SET_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_GET_RSSI_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_GET_DUMP_INFO_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_SET_CUSTOMER_CMD, __tota_general_cmd_handle, false, 0, NULL );
TOTA_COMMAND_TO_ADD(OP_TOTA_GET_CUSTOMER_CMD, __tota_general_cmd_handle, false, 0, NULL );
#ifdef CUSTOMER_APP_BOAT
TOTA_COMMAND_TO_ADD(OP_TOTA_CRC_CHECK_CMD, __tota_general_cmd_handle, false, 0, NULL );
#endif

#ifdef DUMP_LOG_ENABLE
extern uint32_t __log_dump_start[];
#endif

static void general_get_flash_dump_info(TOTA_DUMP_INFO_STRUCT_T * p_flash_info)
{
#ifdef DUMP_LOG_ENABLE
    p_flash_info->address =  (uint32_t)&__log_dump_start;
    p_flash_info->length = LOG_DUMP_SECTION_SIZE;
#else
    p_flash_info->address =  0;
    p_flash_info->length = 0;
#endif
}

#endif

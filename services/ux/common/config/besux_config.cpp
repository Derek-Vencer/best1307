/***************************************************************************
 *
 * Copyright 2015-2023BES.
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
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "cmsis_os.h"
#include "besux_cfg.h"
#include "app_bt.h"

#ifdef BT_MAP_SUPPORT
#include "app_map.h"
#include "bt_map_service.h"
#endif

#if defined(BT_HID_DEVICE)
#include "app_bt_hid.h"
#include "hid_api.h"
#endif

#ifdef BES_OTA
#include "ota_control.h"
#include "ota_spp.h"
#endif

#ifdef __GMA_VOICE__
#include "gma_crypto.h"
#endif



#if defined(BISTO_ENABLED) || defined(__AI_VOICE__)
#include "app_ai_tws.h"

#endif

#ifdef VOICE_DATAPATH
#include "gsound_custom.h"
#include "gsound_custom_tws.h"
#endif

#ifdef __AI_VOICE__
#include "ai_thread.h"
#ifdef __BIXBY_VOICE__
#include "app_bixby_thirdparty_if.h"
#endif
#endif

#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#ifdef __BT_SYNC__
#include "app_bt_sync.h"
#endif

#ifndef BTH_SUBSYS_ONLY
#include "a2dp_decoder.h"
#include "audio_trigger_a2dp.h"
#include "audio_trigger_checker.h"
#include "audio_process_sync.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ibrt_audio_sync.h"
#include "app_bt_media_manager.h"
#include "audio_prompt_sbc.h"
#endif

#ifdef MULTI_DMA_TC
#include "audioflinger.h"
#endif

#ifndef __INTERACTION__
uint8_t g_findme_fadein_vol = 0;
#endif

const struct besux_cfg_t besux_cfg = {
#ifdef BT_HFP_SUPPORT
    .ux_hfp_support = true,
#else
    .ux_hfp_support = false,
#endif

#ifdef BT_MAP_SUPPORT
    .ux_map_support = true,
#else
    .ux_map_support = false,
#endif

#ifdef BT_HID_DEVICE
    .ux_hid_support = true,
#else
    .ux_hid_support = false,
#endif

#ifdef BT_DIP_SUPPORT
    .ux_dip_support = true,
#else
    .ux_dip_support = false,
#endif

#ifdef IBRT_UI_MASTER_ON_TWS_DISCONNECTED
    .ux_set_master_on_tws_disconnected = true,
#else
    .ux_set_master_on_tws_disconnected = false,
#endif

#ifdef BT_ALWAYS_IN_DISCOVERABLE_MODE
    .ux_always_in_discoverable_mode = true,
#else
    .ux_always_in_discoverable_mode = false,
#endif

#ifdef TWS_RS_BY_BTC
    .ux_tws_rs_by_btc_support = true,
#else
    .ux_tws_rs_by_btc_support = false,
#endif

#ifdef TWS_RS_WITHOUT_MOBILE
    .ux_tws_rs_without_mobile = true,
#else
    .ux_tws_rs_without_mobile = false,
#endif

#ifdef ENABLE_ROLE_SWITCH_MONITOR
    .ux_role_switch_monitor = true,
#else
    .ux_role_switch_monitor = false,
#endif

#ifdef IBRT_RIGHT_MASTER
    .ux_set_right_is_master = true,
#else
    .ux_set_right_is_master = false,
#endif

#ifdef USE_SAFE_DISCONNECT
    .ux_use_safe_disconnect = true,
#else
    .ux_use_safe_disconnect = false,
#endif

#ifdef __REPORT_EVENT_TO_CUSTOMIZED_UX__
    .ux_report_evt_to_cudtomux_support = true,
#else
    .ux_report_evt_to_cudtomux_support = false,
#endif

#ifdef CODEC_ERROR_HANDLING
    .ux_codec_error_handling = true,
#else
    .ux_codec_error_handling = false,
#endif

#ifdef BES_OTA
    .ux_bes_ota_support = true,
#else
    .ux_bes_ota_support = false,
#endif

#ifdef __GMA_VOICE__
    .ux_gam_voice_support = true,
#else
    .ux_gam_voice_support = false,
#endif

#ifdef __INTERACTION__
    .ux_interaction_support = true,
#else
    .ux_interaction_support = false,
#endif

#ifdef PROMPT_SELF_MANAGEMENT
    .ux_prompt_self_mgr_support = true,
#else
    .ux_prompt_self_mgr_support = false,
#endif

#ifdef MEDIA_PLAYER_SUPPORT
    .ux_media_player_support = true,
#else
    .ux_media_player_support = false,
#endif

#ifdef __BT_SYNC__
    .ux_bt_sync_support = true,
#else
    .ux_bt_sync_support = false,
#endif

#ifdef CTKD_ENABLE
    .ux_ble_ctkd_support = true,
#else
    .ux_ble_ctkd_support = false,
#endif

#ifdef SUPPORT_GLASSES_PROJECT
    .ux_glasses_project_support = true,
#else
    .ux_glasses_project_support = false,
#endif

#ifdef BISTO_ENABLED
    .ux_bisto_support = true,
#else
    .ux_bisto_support = false,
#endif

#ifdef __AI_VOICE__
    .ux_ai_voice_support = true,
#else
    .ux_ai_voice_support = false,
#endif

#ifdef BT_A2DP_SUPPORT
    .ux_a2dp_support = true,
#else
    .ux_a2dp_support = false,
#endif

#ifdef MULTI_DMA_TC
    .ux_muti_dma_tc_support = true,
#else
    .ux_muti_dma_tc_support = false,
#endif

#ifdef BTH_SUBSYS_ONLY
    .ux_bth_subsys_only_support = true,
#else
    .ux_bth_subsys_only_support = false,
#endif

};

const static struct ux_hfp_support_cb_t ux_hfp_support_cbs = {
#ifdef SUPPORT_SIRI
    .besux_hfp_siri_voice          = app_hfp_siri_voice,
#else
    .besux_hfp_siri_voice          = NULL,
#endif

#ifdef BT_HFP_SUPPORT
    .besux_hfp_is_sco_active       = btapp_hfp_is_sco_active,
    .besux_get_hf_chan_state       = btif_get_hf_chan_state,
    .besux_register_sco_link       = app_ibrt_register_sco_link,
    .besux_restore_hfp_app_ctx     = app_bt_restore_hfp_app_ctx,
    .besux_save_hfp_app_ctx        = app_bt_save_hfp_app_ctx,
    .besux_is_sco_connected        = app_bt_is_sco_connected,
    .besux_send_call_hold_request  = app_hfp_send_call_hold_request,
    .besux_get_call_setup          = btapp_hfp_get_call_setup,
    .besux_hfp_profile_connecting  = btif_hfp_profile_connecting,
    .besux_force_disconnect_hfp_profile     = btif_hfp_force_disconnect_hfp_profile,
    .besux_hfp_profile_restore_ctx          = btif_hfp_profile_restore_ctx,
    .besux_hfp_profile_save_ctx             = btif_hfp_profile_save_ctx,
    .besux_hf_sync_conn_audio_connected     = btif_hf_sync_conn_audio_connected,
    .besux_hf_sync_conn_audio_disconnected  = btif_hf_sync_conn_audio_disconnected,
    .besux_hfp_is_profile_initiator         = btif_hfp_is_profile_initiator,
    .besux_hf_get_negotiated_codec          = btif_hf_get_negotiated_codec,
    .besux_receive_peer_sco_codec_info      = btif_hf_receive_peer_sco_codec_info,
    .besux_hfp_ibrt_service_connected_mock  = hfp_ibrt_service_connected_mock,
    .besux_hfp_update_local_volume          = hfp_update_local_volume,
    .besux_sync_get_hfp_status              = hfp_ibrt_sync_get_status,
    .besux_sync_set_hfp_status              = hfp_ibrt_sync_set_status,
#else
    .besux_hfp_is_sco_active       = NULL,
    .besux_get_hf_chan_state       = NULL,
    .besux_register_sco_link       = NULL,
    .besux_restore_hfp_app_ctx     = NULL,
    .besux_save_hfp_app_ctx        = NULL,
    .besux_is_sco_connected        = NULL,
    .besux_send_call_hold_request  = NULL,
    .besux_get_call_setup          = NULL,
    .besux_hfp_profile_connecting  = NULL,
    .besux_force_disconnect_hfp_profile     = NULL,
    .besux_hfp_profile_restore_ctx          = NULL,
    .besux_hfp_profile_save_ctx             = NULL,
    .besux_hf_sync_conn_audio_connected     = NULL,
    .besux_hf_sync_conn_audio_disconnected  = NULL,
    .besux_hfp_is_profile_initiator         = NULL,
    .besux_hf_get_negotiated_codec          = NULL,
    .besux_receive_peer_sco_codec_info      = NULL,
    .besux_hfp_ibrt_service_connected_mock  = NULL,
    .besux_hfp_update_local_volume          = NULL,
    .besux_sync_get_hfp_status              = NULL,
    .besux_sync_set_hfp_status              = NULL,
#endif
};

const static struct ux_map_support_cb_t ux_map_support_cbs = {
#ifdef BT_MAP_SUPPORT
    .besux_map_profile_save_ctx             = btif_map_profile_save_ctx,
    .besux_save_map_app_ctx                 = app_bt_save_map_app_ctx,
    .besux_map_profile_restore_ctx          = btif_map_profile_restore_ctx,
    .besux_restore_map_app_ctx              = app_bt_restore_map_app_ctx,
#else
    .besux_map_profile_save_ctx             = NULL,
    .besux_save_map_app_ctx                 = NULL,
    .besux_map_profile_restore_ctx          = NULL,
    .besux_restore_map_app_ctx              = NULL,
#endif
};

const static struct ux_hid_support_cb_t ux_hid_support_cbs = {
#ifdef BT_HID_DEVICE
    .besux_hid_profile_save_ctx             = btif_hid_profile_save_ctx,
    .besux_save_hid_app_ctx                 = app_bt_save_hid_app_ctx,
    .besux_hid_profile_restore_ctx          = btif_hid_profile_restore_ctx,
    .besux_restore_hid_app_ctx              = app_bt_restore_hid_app_ctx,
    .besux_hid_profile_connect              = app_bt_hid_profile_connect,
    .besux_hid_profile_disconnect           = app_bt_hid_profile_disconnect,
#else
    .besux_hid_profile_save_ctx             = NULL,
    .besux_save_hid_app_ctx                 = NULL,
    .besux_hid_profile_restore_ctx          = NULL,
    .besux_restore_hid_app_ctx              = NULL,
    .besux_hid_profile_connect              = NULL,
    .besux_hid_profile_disconnect           = NULL,
#endif
};

const static struct ux_use_safe_disconnect_cb_t ux_safe_disconnect_cbs = {
#if defined(USE_SAFE_DISCONNECT)
    .besux_custom_stop_ibrt_ongoing         = stop_ibrt_ongoing,
    .besux_custom_all_safe_disconnect       = app_custom_ui_all_safe_disconnect,
#else
    .besux_custom_stop_ibrt_ongoing         = NULL,
    .besux_custom_all_safe_disconnect       = NULL,
#endif
} ;

const static struct ux_dip_support_cb_t ux_dip_support_cbs = {
#ifdef BT_DIP_SUPPORT
    .besux_dip_get_device_info                      = btif_dip_get_device_info,
    .besux_dip_sync_init                            = app_dip_sync_init,
    .besux_register_dip_info_queried_callback       = app_dip_register_dip_info_queried_callback,
#else
    .besux_dip_get_device_info                      = NULL,
    .besux_dip_sync_init                            = NULL,
    .besux_register_dip_info_queried_callback       = NULL,
#endif
};

const static struct ux_besota_support_cb_t ux_besota_support_cbs = {
#ifdef BES_OTA
    .besux_set_ota_role_switch_initiator             = app_set_ota_role_switch_initiator,
    .besux_ota_send_role_switch_req                  = bes_ota_send_role_switch_req,
    .besux_get_bes_ota_state                         = app_get_bes_ota_state,
    .besux_ota_is_in_progress                        = ota_is_in_progress,
    .besux_ota_send_role_switch_complete             = ota_control_send_role_switch_complete,
#else
    .besux_set_ota_role_switch_initiator             = NULL,
    .besux_ota_send_role_switch_req                  = NULL,
    .besux_get_bes_ota_state                         = NULL,
    .besux_ota_is_in_progress                        = NULL,
    .besux_ota_send_role_switch_complete             = NULL,
#endif
};

const static struct ux_other_macro_support_cb_t other_macro_support_cbs = {
#ifdef __GMA_VOICE__
    .besux_gma_secret_key_send                      = gma_secret_key_send,
#else
    .besux_gma_secret_key_send                      = NULL,
#endif

#ifdef __BIXBY
    .besux_bixby_hotword_detect_value_get           = app_bixby_hotword_detect_value_get,
#else
    .besux_bixby_hotword_detect_value_get           = NULL,
#endif

#ifdef MEDIA_PLAYER_SUPPORT
    .besux_media_PlayAudio                          = media_PlayAudio,
    .besux_trigger_media_play                       = trigger_media_play,
    .besux_media_PlayAudio_standalone               = media_PlayAudio_standalone,
#else
    .besux_media_PlayAudio                          = NULL,
    .besux_trigger_media_play                       = NULL,
    .besux_media_PlayAudio_standalone               = NULL,
#endif
};

const static struct ux_ai_voice_support_cb_t ux_ai_voice_support_cbs = {
#ifdef __AI_VOICE__
    .besux_ai_tws_role_switch_prepare               = app_ai_tws_role_switch_prepare,
    .besux_ai_tws_role_switch                       = app_ai_tws_role_switch,
    .besux_ai_tws_master_role_switch_prepare        = app_ai_tws_master_role_switch_prepare,
    .besux_ai_tws_role_switch_prepare_done          = app_ai_tws_role_switch_prepare_done,
    .besux_ai_if_mobile_connect_handle              = app_ai_if_mobile_connect_handle,
    .besux_ai_tws_gsound_sync_init                  = app_ai_tws_gsound_sync_init,
    .besux_ai_tws_role_switch_complete              = app_ai_tws_role_switch_complete,
    .besux_ai_tws_sync_init                         = app_ai_tws_sync_init,
    .besux_ai_tws_get_local_role                    = app_ai_tws_get_local_role,
#else
    .besux_ai_tws_role_switch_prepare               = NULL,
    .besux_ai_tws_role_switch                       = NULL,
    .besux_ai_tws_master_role_switch_prepare        = NULL,
    .besux_ai_tws_role_switch_prepare_done          = NULL,
    .besux_ai_if_mobile_connect_handle              = NULL,
    .besux_ai_tws_gsound_sync_init                  = NULL,
    .besux_ai_tws_role_switch_complete              = NULL,
    .besux_ai_tws_sync_init                         = NULL,
    .besux_ai_tws_get_local_role                    = NULL,
#endif

#ifdef BISTO_ENABLED
    .besux_tws_update_roleswitch_initiator          = gsound_tws_update_roleswitch_initiator,
    .besux_tws_request_roleswitch                   = gsound_tws_request_roleswitch,
    .besux_gsound_set_ble_connect_state             = gsound_set_ble_connect_state,
    .besux_gsound_on_bt_link_disconnected           = gsound_on_bt_link_disconnected,
    .besux_gsound_on_system_role_switch_done        = gsound_on_system_role_switch_done,
    .besux_gsound_tws_role_update                   = gsound_tws_role_update,
#else
    .besux_tws_update_roleswitch_initiator          = NULL,
    .besux_tws_request_roleswitch                   = NULL,
    .besux_gsound_set_ble_connect_state             = NULL,
    .besux_gsound_on_bt_link_disconnected           = NULL,
    .besux_gsound_on_system_role_switch_done        = NULL,
    .besux_gsound_tws_role_update                   = NULL,
#endif
};

const static struct ux_bt_sync_support_cb_t ux_bt_sync_support_cbs = {
#if  defined(__BT_SYNC__) 
    .besux_bt_sync_enable                          = app_bt_sync_enable,
    .besux_bt_sync_tws_cmd_handler                 = app_bt_sync_tws_cmd_handler,
    .besux_bt_sync_send_tws_cmd_done               = app_bt_sync_send_tws_cmd_done,
#else
    .besux_bt_sync_enable                          = NULL,
    .besux_bt_sync_tws_cmd_handler                 = NULL,
    .besux_bt_sync_send_tws_cmd_done               = NULL,
#endif
};

#ifndef BTH_SUBSYS_ONLY
extern void app_bt_stream_ibrt_set_trigger_time(uint8_t device_id, APP_TWS_IBRT_AUDIO_SYNC_TRIGGER_T *sync_trigger);
extern int bt_sco_chain_set_master_role(bool is_master);
#endif

const struct ux_hfp_support_cb_t *besux_get_hfp_support_cbs(void)
{
    return &ux_hfp_support_cbs;
}

const struct ux_map_support_cb_t *besux_get_map_support_cbs(void)
{
    return &ux_map_support_cbs;
}

const struct ux_hid_support_cb_t *besux_get_hid_support_cbs(void)
{
     return &ux_hid_support_cbs;
}

const struct ux_use_safe_disconnect_cb_t *besux_get_safe_disconnect_cbs(void)
{
    return &ux_safe_disconnect_cbs;
}

const struct ux_dip_support_cb_t *besux_get_dip_support_cbs(void)
{
    return &ux_dip_support_cbs;
}

const struct ux_besota_support_cb_t *besux_get_besota_support_cbs(void)
{
    return &ux_besota_support_cbs;
}

const struct ux_other_macro_support_cb_t *besux_get_other_macro_support_cbs(void)
{
    return &other_macro_support_cbs;
}

const struct ux_ai_voice_support_cb_t *besux_get_ai_voice_support_cbs(void)
{
    return &ux_ai_voice_support_cbs;
}

const struct ux_bt_sync_support_cb_t *besux_get_bt_sync_support_cbs(void)
{
    return &ux_bt_sync_support_cbs;
}

/**
 * ux to Audioplayers
 */

int besux_to_audio_a2dp_sync_init(float ratio)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sync_init(ratio);
#endif
}

int besux_to_audio_a2dp_reset_data(void)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sync_reset_data();
#endif
}

int besux_to_audio_a2dp_sysfreq_boost_running(void)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sysfreq_boost_running();
#endif
}

int besux_to_audio_a2dp_sysfreq_boost_start(uint32_t boost_cnt,bool is_init_boost)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sysfreq_boost_start(boost_cnt, is_init_boost);
#endif
}

int besux_to_audio_a2dp_latency_factor_set(float factor)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_latency_factor_set(factor);
#endif
}

int besux_to_audio_a2dp_sync_tune_cancel(void)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sync_tune_cancel();
#endif
}

int besux_to_audio_a2dp_sync_tune_sample_rate(float ratio)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sync_tune_sample_rate(ratio);
#endif
}

void besux_to_audio_local_scalable_sbm_feature_handler(uint8_t device_id, bool isEnable)
{
#ifndef BTH_SUBSYS_ONLY
    sbm_control_local_scalable_sbm_feature_handler(device_id, isEnable);
#endif
}

void besux_to_audio_local_scalable_sbm_feature_updated_callback(uint8_t device_id, bool isEnable, bool isSuccessful)
{
#ifndef BTH_SUBSYS_ONLY
    sbm_local_scalable_sbm_feature_updated_callback(device_id, isEnable, isSuccessful);
#endif
}

bool besux_to_audio_a2dp_retrigger_is_on_process(void)
{
#ifdef BTH_SUBSYS_ONLY
    return false;
#else
    return a2dp_audio_retrigger_is_on_process();
#endif
}

void besux_to_audio_a2dp_retrigger_set_on_process(bool flag)
{
#ifndef BTH_SUBSYS_ONLY
    a2dp_audio_retrigger_set_on_process(flag);
#endif
}

int besux_to_audio_a2dp_stream_trigger_checker_stop(void)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return app_bt_stream_trigger_checker_stop();
#endif
}

int besux_to_audio_a2dp_detect_next_packet_callback_register(besux_audio_detect_next_packet_callback callback)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_detect_next_packet_callback_register(callback);
#endif
}

int besux_to_audio_a2dp_detect_store_packet_callback_register(besux_audio_detect_next_packet_callback callback)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_detect_store_packet_callback_register(callback);
#endif
}

int besux_to_audio_a2dp_sync_direct_tune_sample_rate(float ratio)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return a2dp_audio_sync_direct_tune_sample_rate(ratio);
#endif
}

void besux_to_audio_play_speed_tuning_req_process(uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen)
{
#ifndef BTH_SUBSYS_ONLY
    app_ibrt_custom_play_speed_tuning_req_process(rsp_seq, ptrParam, paramLen);
#endif
}

void besux_to_audio_ibrt_sync_target_buf_ms_req_process(uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen)
{
#ifndef BTH_SUBSYS_ONLY
    app_ibrt_sync_target_buf_ms_req_process(rsp_seq, ptrParam, paramLen);
#endif
}

void besux_to_audio_ibrt_mobile_link_playback_info_receive(uint8_t device_id,
        APP_TWS_IBRT_AUDIO_SYNC_TRIGGER_T *sync_trigger)
{
#ifndef BTH_SUBSYS_ONLY
    app_bt_stream_ibrt_mobile_link_playback_info_receive(device_id, sync_trigger);
#endif
}

bool besux_to_audio_stream_trigger_onprocess(void)
{
#ifdef BTH_SUBSYS_ONLY
    return false;
#else
    return app_bt_stream_trigger_onprocess();
#endif
}

void besux_to_audio_stream_ibrt_set_trigger_time(uint8_t device_id, APP_TWS_IBRT_AUDIO_SYNC_TRIGGER_T *sync_trigger)
{
#ifndef BTH_SUBSYS_ONLY
    app_bt_stream_ibrt_set_trigger_time(device_id, sync_trigger);
#endif
}

int besux_to_audio_sco_chain_set_master_role(bool is_master)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return bt_sco_chain_set_master_role(is_master);
#endif
}

void besux_to_audio_sync_mix_prompt_req_handler(uint8_t* ptrParam, uint16_t paramLen)
{
#ifdef MEDIA_PLAYER_SUPPORT
    app_ibrt_sync_mix_prompt_req_handler(ptrParam, paramLen);
#endif
}

void besux_to_audio_tws_cmd_prompt_play_req_handler(uint8_t *ptrParam, uint32_t paramLen)
{
#ifdef MEDIA_PLAYER_SUPPORT
    app_tws_cmd_prompt_play_req_handler(ptrParam, paramLen);
#endif
}

int besux_to_audio_manager_sendrequest_need_callback(uint8_t massage_id, uint16_t stream_type, uint8_t device_id,
                                                    uint32_t aud_id, uint32_t cb, uint32_t cb_param)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return app_audio_manager_sendrequest_need_callback(massage_id, stream_type, device_id, aud_id, cb, cb_param);
#endif
}

int32_t besux_to_audio_process_tws_sync_change(uint8_t *buf, uint32_t len)
{
#ifdef BTH_SUBSYS_ONLY
    return 0;
#else
    return audio_process_tws_sync_change(buf, len);
#endif
}

uint8_t besux_to_audio_af_stream_get_dma_chan(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#if defined(MULTI_DMA_TC) && !defined(BTH_SUBSYS_ONLY)
    return af_stream_get_dma_chan(id, stream);
#else
    return 0;
#endif
}

uint32_t besux_to_audio_af_stream_get_dma_base_addr(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream)
{
#if defined(MULTI_DMA_TC) && !defined(BTH_SUBSYS_ONLY)
    return af_stream_get_dma_base_addr(id, stream);
#else
    return 0;
#endif
}
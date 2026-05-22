
#include <stdbool.h>
#include <stdint.h>
#include "bluetooth.h"
#include "hfp_i.h"
#include "btm_i.h"
#include "sdp_i.h"
#include "rfcomm_i.h"
#include "bt_callback_func.h"
#include "bth_patch.h"
#include "patch.h"
#include "hal_location.h"

#ifndef BLE_ONLY_ENABLED

#define HFP_LOCAL_VERSION 0x0108
#define HSP_LOCAL_VERSION 0x0102

#define MAX_NUMBER_LEN 33
#define MAX_OPERATOR_NAME_LEN 17
#define MAX_OUT_ATTR_COUNT 2


#define AG_CFG_MAX_RX_CREDIT 7
#define AG_CFG_CREDIT_GIVE_STEP 3

#define HF_CFG_MAX_RX_CREDIT 207
#define HF_CFG_CREDIT_GIVE_STEP 20

extern uint8 _hfp_sdp_registered;
extern hfp_callback_t hfp_indicate_callback;

extern struct hshf_control* (*g_hshf_req_acceptable_callback)(uint8_t device_id, const bt_bdaddr_t* remote, uint8_t server_channel);
extern struct hshf_control* (*g_hshf_search_channel_callback)(uint8_t device_id);

extern int hfp_rfcomm_callback(const bt_bdaddr_t *remote, bt_socket_event_t event, bt_socket_callback_param_t param);

const uint8 _hfp_class_id_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
        SDP_DE_UUID_H1_D2,
            SERV_UUID_HandsFree, // hfp uuid
        SDP_DE_UUID_H1_D2,
            SERV_UUID_GENERIC_AUDIO,
};

const uint8 _hfp_protocol_descriptor_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(12), // attr value head
        SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_L2CAP, // l2cap uuid
        SDP_DE_DESEQ_8BITSIZE_H2_D(5), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_RFCOMM, // rfcomm uuid
            SDP_DE_UINT_H1_D1,
                HF_CFG_SERVER_CHANNEL, // port
};

const uint8 _hfp_browse_group[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
        SDP_DE_UUID_H1_D2, // attr value head
            0x10,0x02,
};
const uint8 _hfp_bt_profile_descriptor_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(8), // attr value head
        SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_HandsFree, // hfp uuid
            SDP_DE_UINT_H1_D2,
                SDP_SPLIT_16BITS_BE(HFP_LOCAL_VERSION),
};

uint8 _hfp_support_features_list[] = {
    SDP_DE_UINT_H1_D2,
        SDP_SPLIT_16BITS_BE(HFP_HF_SDP_FEATURES),
};

const uint8 _hfp_lang_base_id_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(9),
        SDP_DE_UINT_H1_D2,
            0x65,0x6e,
        SDP_DE_UINT_H1_D2,
            0x00,0x6a,
        SDP_DE_UINT_H1_D2,
            0x01,0x00,
};

const bt_sdp_record_attr_t _hfp_sdp_attrs[] = { // list attr id in ascending order
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_CLASS_ID_LIST, _hfp_class_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROTOCOL_DESC_LIST, _hfp_protocol_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BROWSE_GROUP_LIST, _hfp_browse_group),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_LANG_BASE_ID_LIST, _hfp_lang_base_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BT_PROFILE_DESC_LIST, _hfp_bt_profile_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SUPPORTED_FEATURES, _hfp_support_features_list),
};

void SRAM_TEXT_LOC hfp_init_patch(hfp_callback_t callback,
        struct hshf_control* (*accept)(uint8_t device_id, const bt_bdaddr_t* addr, uint8 server_channel),
        struct hshf_control* (*search)(uint8_t device_id))
{
    TRACE(0, "hfp_init_patch");
    bt_status_t status = BT_STS_FAILED;
    if (_hfp_sdp_registered)
    {
        return;
    }
    _hfp_sdp_registered = true;
    if (!bt_callback_cfg_apple_hf_at_support() || bt_callback_cfg_hf_support_hf_ind_feature())
    {
        hshf_set_hf_indicator_enabled(true);
    }
    hfp_indicate_callback = callback;
    g_hshf_req_acceptable_callback = accept;
    g_hshf_search_channel_callback = search;
#ifdef BT_HFP_AG_ROLE
    if (bt_callback_cfg_bt_source_enable())
    {
        status = rfcomm_create_port(AG_CFG_SERVER_CHANNEL,  call_get_func(btprf, hfp_rfcomm_callback), _hfp_ag_sdp_attrs, ARRAY_SIZE(_hfp_ag_sdp_attrs));
        if (status == BT_STS_FAILED)
        {
            DEBUG_WARNING(0, "hf_init: ag create port failed");
            return;
        }
        rfcomm_set_rx_credits(AG_CFG_SERVER_CHANNEL, false, AG_CFG_MAX_RX_CREDIT, AG_CFG_CREDIT_GIVE_STEP);
        rfcomm_listen(AG_CFG_SERVER_CHANNEL, true, (bt_socket_accept_callback_t) call_get_func(btprf, hfp_rfcomm_callback));
#ifdef HSP_ENABLE
        if (bt_callback_cfg_hsp_enable())
        {
            status = rfcomm_create_port(HSP_AG_SERVER_CHANNEL,  call_get_func(btprf, hfp_rfcomm_callback), _hsp_ag_sdp_attrs, ARRAY_SIZE(_hsp_ag_sdp_attrs));
            if (status == BT_STS_FAILED)
            {
                DEBUG_WARNING(0, "hf_init: hsp ag create port failed");
                return;
            }
            rfcomm_set_rx_credits(HSP_AG_SERVER_CHANNEL, false, AG_CFG_MAX_RX_CREDIT, AG_CFG_CREDIT_GIVE_STEP);
            rfcomm_listen(HSP_AG_SERVER_CHANNEL, true, (bt_socket_accept_callback_t) call_get_func(btprf, hfp_rfcomm_callback));
        }
#endif
    }
#endif
    if (bt_callback_cfg_bt_sink_enable())
    {
        uint16_t hf_sdp_features = HFP_HF_SDP_FEATURES;
        if (bt_callback_cfg_hf_dont_support_cli_feature())
        {
            hf_sdp_features &= (~(HFP_HF_FEAT_CLI));
        }
        if (bt_callback_cfg_hf_dont_support_enhanced_call())
        {
            hf_sdp_features &= (~(HFP_HF_FEAT_ECC|HFP_HF_FEAT_ECS));
            hf_sdp_features |= HFP_HF_SDP_FEAT_WBS;
        }
        _hfp_support_features_list[1] = (uint8_t)((hf_sdp_features>>8)&0xff);
        _hfp_support_features_list[2] = (uint8_t)(hf_sdp_features&0xff);
        status = rfcomm_create_port(HF_CFG_SERVER_CHANNEL,  call_get_func(btprf, hfp_rfcomm_callback), _hfp_sdp_attrs, ARRAY_SIZE(_hfp_sdp_attrs));
        if (status == BT_STS_FAILED)
        {
            DEBUG_WARNING(0, "hf_init: hf create port failed");
            return;
        }
        rfcomm_set_rx_credits(HF_CFG_SERVER_CHANNEL, false, HF_CFG_MAX_RX_CREDIT, HF_CFG_CREDIT_GIVE_STEP);
        rfcomm_listen(HF_CFG_SERVER_CHANNEL, true, (bt_socket_accept_callback_t) call_get_func(btprf, hfp_rfcomm_callback));
#ifdef HSP_ENABLE
        if (bt_callback_cfg_hsp_enable())
        {
            status = rfcomm_create_port(HSP_HS_SERVER_CHANNEL,  call_get_func(btprf, hfp_rfcomm_callback), _hsp_sdp_attrs, ARRAY_SIZE(_hsp_sdp_attrs));
            if (status == BT_STS_FAILED)
            {
                DEBUG_WARNING(0, "hf_init: hsp hs create port failed");
                return;
            }
            rfcomm_set_rx_credits(HSP_HS_SERVER_CHANNEL, false, HF_CFG_MAX_RX_CREDIT, HF_CFG_CREDIT_GIVE_STEP);
            rfcomm_listen(HSP_HS_SERVER_CHANNEL, true, (bt_socket_accept_callback_t) call_get_func(btprf, hfp_rfcomm_callback));
        }
#endif
    }
}
void *p_hfp_init_patch;

void hfp_patch()
{
    p_hfp_init_patch = (void *)hfp_init_patch;
    patch_open(PATCH_CTRL_ID_1, 0);
    // todo
    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x14db98, (uint32_t)p_hfp_init_patch);
    TRACE(0, "patch: enable p_hfp_init_patch");
}

#endif /* BLE_ONLY_ENABLED */
/***************************************************************************
*
* Copyright 2015-2024 BES.
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
#include "hal_trace.h"
#include "btm_i.h"
#include "hci_i.h"
#include "bt_if.h"
#include "stdarg.h"
#include "nvrecord_bt.h"
#include "avrcp_i.h"
#include "patch.h"
#include "hal_location.h"

#ifndef BLE_ONLY_ENABLED

#define AVCTP_LOCAL_VERSION 0x0104
#define AVRCP_LOCAL_VERSION 0x0106

const uint8 _avrcp_ct_browse_group[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
        SDP_DE_UUID_H1_D2, // attr value head
            0x10,0x02,
};

const uint8 _avrcp_ct_class_id_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
        SDP_DE_UUID_H1_D2,
            SERV_UUID_AV_REMOTE_CONTROL, // avrcp ct uuid
        SDP_DE_UUID_H1_D2,
            SERV_UUID_AV_REMOTE_CONTROL_CONTROLLER,
};

const uint8 _avrcp_tg_class_id_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
        SDP_DE_UUID_H1_D2,
            SERV_UUID_AV_REMOTE_CONTROL_TARGET, // avrcp tg uuid
};

const uint8 _avrcp_ct_protocol_descriptor_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(16), // attr value head
        SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_L2CAP, // l2cap uuid
            SDP_DE_UINT_H1_D2,
                SERV_UUID_AVCTP, // avctp psm
        SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_AVCTP, // acdtp uuid
            SDP_DE_UINT_H1_D2,
                SDP_SPLIT_16BITS_BE(AVCTP_LOCAL_VERSION) // version
};

const uint8 _avrcp_ct_bt_profile_descriptor_list[] = {
    SDP_DE_DESEQ_8BITSIZE_H2_D(8), // attr value head
        SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
            SDP_DE_UUID_H1_D2,
                SERV_UUID_AV_REMOTE_CONTROL, // a2dp psm
            SDP_DE_UINT_H1_D2,
                SDP_SPLIT_16BITS_BE(AVRCP_LOCAL_VERSION)
};


const uint8 _avrcp_ct_supported_features[] = {
    SDP_DE_UINT_H1_D2,
        0x00,0x01,
};

const uint8 _avrcp_tg_supported_features[] = {
    SDP_DE_UINT_H1_D2,
        0x00,0x02,
};


const uint8 _avrcp_ct_record_state[] = {
    0x0a,
        0x00,0x00,0x44,0x0e
};

const uint8 _avrcp_ct_service_name[] = {
    SDP_DE_TEXTSTR_8BITSIZE_H2_D(14),          /* Null terminated text string */
    'A', 'd', 'v', 'a', 'n', 'c', 'e', 'd', ' ', 'A', 'u',
    'd', 'i', 'o'

};

const uint8 _avrcp_tg_service_name[] = {
    SDP_DE_TEXTSTR_8BITSIZE_H2_D(27),          /* Null terminated text string */
    'A', 'V', 'R', 'C', 'P', ' ', 'R', 'e', 'm', 'o', \
    't', 'e', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', \
    ' ', 'T', 'a', 'r', 'g','e', 't'
};

const uint8 _avrcp_ct_provider_name[] = {
    SDP_DE_TEXTSTR_8BITSIZE_H2_D(6),          /* Null terminated text string */
    'M', 't', 'V', 'i', 'e', 'w'
};

const uint8 _avrcp_tg_additional_prot_desc_list[] = {
#if 0
    SDP_ATTRIB_HEADER_8BIT(18),  /* Data element sequence, 14 bytes  */

    /* Each element of this list is a protocol descriptor list.  For AVRCP,
     * there is only one list.
     */

    SDP_ATTRIB_HEADER_8BIT(16),  /* Data element sequence, 16 bytes */

    /* Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.  The second element is the AVCTP version,
     * which has only a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),             /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_L2CAP),            /* Uuid16         */
    SDP_UINT_16BIT(PROT_AVCTP_BROWSING),   /* Browsing PSM   */

    /* The next protocol descriptor is for Browsing. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_AVCTP),     /* Uuid16         */
    SDP_UINT_16BIT(0x0102)          /* Uint16 Version */
#endif
0x35,0x12,0x35,0x10,0x35,0x06,0x19,0x01,0x00,0x09,0x00,0x1b,0x35,0x06,0x19,0x00,0x17,0x09,
    SDP_SPLIT_16BITS_BE(AVCTP_LOCAL_VERSION)

};

static const bt_sdp_record_attr_t _avrcp_ct_sdp_attrs[] = { // list attr id in ascending order
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_CLASS_ID_LIST, _avrcp_ct_class_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_RECORD_STATE, _avrcp_ct_record_state),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROTOCOL_DESC_LIST, _avrcp_ct_protocol_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BROWSE_GROUP_LIST, _avrcp_ct_browse_group),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BT_PROFILE_DESC_LIST, _avrcp_ct_bt_profile_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_NAME, _avrcp_ct_service_name),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROVIDER_NAME, _avrcp_ct_provider_name),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SUPPORTED_FEATURES, _avrcp_ct_supported_features),
};

static const bt_sdp_record_attr_t _avrcp_tg_sdp_attrs[] = { // list attr id in ascending order
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_CLASS_ID_LIST, _avrcp_tg_class_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROTOCOL_DESC_LIST, _avrcp_ct_protocol_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BROWSE_GROUP_LIST, _avrcp_ct_browse_group),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BT_PROFILE_DESC_LIST, _avrcp_ct_bt_profile_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_ADDITIONAL_PROT_DESC_LISTS, _avrcp_tg_additional_prot_desc_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_NAME, _avrcp_tg_service_name),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROVIDER_NAME, _avrcp_ct_provider_name),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SUPPORTED_FEATURES, _avrcp_tg_supported_features),
};

extern uint8 _avrcp_ct_sdp_registered;

int8 SRAM_TEXT_LOC avrcp_turnOn_patch(struct avrcp_control_t *avrcp_ctl,
        struct avctp_control_t* (*accept)(uint8_t device_id, void* remote),
        struct avctp_control_t* (*search)(uint8_t device_id, uint32 l2cap_handle))
{
    int8 ret = FAILURE;

    /* !only patch for sdp here */
    if (_avrcp_ct_sdp_registered == 0) {
        sdp_create_record(_avrcp_ct_sdp_attrs, ARRAY_SIZE(_avrcp_ct_sdp_attrs));
        sdp_create_record(_avrcp_tg_sdp_attrs, ARRAY_SIZE(_avrcp_tg_sdp_attrs));
        _avrcp_ct_sdp_registered = 1;
    }

    avrcp_init_inst(avrcp_ctl, NULL, NULL);

    avctp_init(&avrcp_ctl->avctp_ctl, accept, search);

    ret = avctp_register_server(&avrcp_ctl->avctp_ctl, AVCTP_CFG_SERVER_CHANNEL,  avrcp_notify_callback,  avrcp_datarecv_callback);

    avrcp_setState(avrcp_ctl, AVRCP_STANDBY);

    return ret;
}

void *p_avrcp_turnOn_patch;

void avrcp_patch()
{
    p_avrcp_turnOn_patch = (void *)avrcp_turnOn_patch;

    patch_open(PATCH_CTRL_ID_1, 0);

    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x151374, (uint32_t)p_avrcp_turnOn_patch);

    TRACE(0, "patch: enable avrcp_turnOn_patch");
}

#endif /* BLE_ONLY_ENABLED */
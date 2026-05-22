#include "bluetooth.h"
#include "a2dp_i.h"
#include "avrcp_i.h"
#include "btm_i.h"
#include "l2cap_i.h"
#include "sdp_i.h"
#include "bt_if.h"
#include "patch.h"
#include "hal_location.h"

#ifndef BLE_ONLY_ENABLED

#define A2DP_VER_1_2 0x0102
#define A2DP_VER_1_3 0x0103
#define A2DP_VER_1_4 0x0104

#undef A2DP_LOCAL_VERSION
#define A2DP_LOCAL_VERSION A2DP_VER_1_4

const uint8 _a2dp_sink_browse_group[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
    SDP_DE_UUID_H1_D2, // attr value head
    0x10,0x02,
};

const uint8 _a2dp_sink_class_id_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AUDIOSINK,
};

const uint8 _a2dp_sink_protocol_descriptor_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(16), // attr value head
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_L2CAP, // l2cap uuid
    SDP_DE_UINT_H1_D2,
    SERV_UUID_AVDTP, // avdtp psm
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AVDTP, // avdtp uuid
    SDP_DE_UINT_H1_D2,
    SDP_SPLIT_16BITS_BE(AVDTP_LOCAL_VERSION) // version
};

const uint8 _a2dp_sink_bt_profile_descriptor_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(8), // attr value head
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AdvancedAudioDistribution, // a2dp psm
    SDP_DE_UINT_H1_D2,
    SDP_SPLIT_16BITS_BE(A2DP_LOCAL_VERSION)
};

const uint8 _a2dp_sink_support_features_list[] =
{
    SDP_DE_UINT_H1_D2,
    0x00,0x03,
};

static const bt_sdp_record_attr_t _a2dp_sink_sdp_attrs[] = { // list attr id in ascending order
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_CLASS_ID_LIST, _a2dp_sink_class_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROTOCOL_DESC_LIST, _a2dp_sink_protocol_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BROWSE_GROUP_LIST, _a2dp_sink_browse_group),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BT_PROFILE_DESC_LIST, _a2dp_sink_bt_profile_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SUPPORTED_FEATURES, _a2dp_sink_support_features_list),
};

const uint8 _a2dp_source_browse_group[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
    SDP_DE_UUID_H1_D2, // attr value head
    0x10,0x02,
};

const uint8 _a2dp_source_class_id_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(3), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AUDIOSOURCE,
};

const uint8 _a2dp_source_protocol_descriptor_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(16), // attr value head
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_L2CAP, // l2cap uuid
    SDP_DE_UINT_H1_D2,
    SERV_UUID_AVDTP, // avdtp psm
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AVDTP, // avdtp uuid
    SDP_DE_UINT_H1_D2,
    SDP_SPLIT_16BITS_BE(AVDTP_LOCAL_VERSION) // version
};

const uint8 _a2dp_source_bt_profile_descriptor_list[] =
{
    SDP_DE_DESEQ_8BITSIZE_H2_D(8), // attr value head
    SDP_DE_DESEQ_8BITSIZE_H2_D(6), // attr value head
    SDP_DE_UUID_H1_D2,
    SERV_UUID_AdvancedAudioDistribution, // a2dp psm
    SDP_DE_UINT_H1_D2,
    SDP_SPLIT_16BITS_BE(A2DP_LOCAL_VERSION)
};

const uint8 _a2dp_source_support_features_list[] =
{
    SDP_DE_UINT_H1_D2,
    0x00,0x03,
};

static const bt_sdp_record_attr_t _a2dp_source_sdp_attrs[] = { // list attr id in ascending order
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SERVICE_CLASS_ID_LIST, _a2dp_source_class_id_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_PROTOCOL_DESC_LIST, _a2dp_source_protocol_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BROWSE_GROUP_LIST, _a2dp_source_browse_group),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_BT_PROFILE_DESC_LIST, _a2dp_source_bt_profile_descriptor_list),
    SDP_DEF_ATTRIBUTE(SERV_ATTRID_SUPPORTED_FEATURES, _a2dp_source_support_features_list),
};

bool source_sdp_initialized = false;
bool sink_sdp_initialized = false;

int8 p_av_sdp_init(struct a2dp_control_t *stream)
{
    TRACE(0, "p_av_sdp_init");
    if (!source_sdp_initialized)
    {
        source_sdp_initialized = true;
        if (bt_callback_cfg_bt_source_enable())
        {
            sdp_create_record(_a2dp_source_sdp_attrs, ARRAY_SIZE(_a2dp_source_sdp_attrs));
        }
    }

    if (!sink_sdp_initialized)
    {
        sink_sdp_initialized = true;
        if (bt_callback_cfg_bt_sink_enable() && bt_callback_cfg_a2dp_sink_enable())
        {
            sdp_create_record(_a2dp_sink_sdp_attrs, ARRAY_SIZE(_a2dp_sink_sdp_attrs));
        }
    }

    return 0;
}

void SRAM_TEXT_LOC a2dp_stack_init_patch(struct avdtp_control_t* (*search)(uint8_t device_id, bool is_channel_req_context))
{
    avdtp_init(search);
    p_av_sdp_init(NULL);
}

void *p_a2dp_stack_init_patch;

void a2dp_patch()
{
    p_a2dp_stack_init_patch = (void *)a2dp_stack_init_patch;

    patch_open(PATCH_CTRL_ID_1, 0);


    // todo
    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x14c198, (uint32_t)p_a2dp_stack_init_patch);

    TRACE(0, "patch: enable p_av_sdp_init_patch");
}

#endif /* BLE_ONLY_ENABLED */

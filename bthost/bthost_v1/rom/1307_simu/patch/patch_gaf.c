/***************************************************************************
*
* Copyright 2015-2023 BES.
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
#include "bth_patch.h"
#include "hal_trace.h"
#include "stdarg.h"

typedef struct bap_uc_srv_stream
{
    /// stream node
    struct list_node node_head;
    /// Connection local index
    uint8_t con_lid;
    /// CIS ID
    uint8_t cis_id;
    /// CIS HANDLE
    uint16_t cis_hdl;
    /// ASE ID for each direction
    uint8_t ase_id[2];
    /// Group stream belong to
    void *p_grp;
    /// Stream state bitfiled
    uint8_t state_bf;
} bap_uc_srv_stream_t;

extern int bt_rom_printf(uint32_t attr, const char *fmt, va_list ap);
extern int bap_uc_srv_bap_stream_event_callback(uintptr_t group_id, bap_event_t event, bap_event_param_t param);
extern bap_uc_srv_stream_t *bap_uc_srv_get_exist_stream_by_cis_hdl(uint8_t con_lid, uint16_t cis_hdl);
extern void bap_uc_srv_handle_stream_event(bap_uc_srv_stream_t *p_stream, uint8_t event, uint16_t status);

extern uint32_t g_patch_tbl_gaf[];

/*PATCH 1*/
static int gaf_log_func_internal_patch(uint8_t log_lvl, const char *format, va_list var_args)
{
    return bt_rom_printf(TR_MOD(BLESTACK), format, var_args);
}

/*PATCH 2*/
static int bap_uc_srv_bap_stream_event_callback_patch(uintptr_t group_id, bap_event_t event, bap_event_param_t param)
{
    uint8_t con_lid = 0;
    gap_conn_item_t *p_acl_conn = NULL;
    bap_uc_srv_stream_t *p_stream = NULL;
    uint8_t iso_state_bf = 0b11;//BAP_UC_SRV_STREAM_STATE_ISO_DP_SETUP_MASK

    if (event == BAP_EVENT_CIS_CLOSED)
    {
        p_acl_conn =
            gap_get_conn_item(param.cis_closed->stream->connhdl);

        if (p_acl_conn == NULL)
        {
            return BT_STS_NO_LINK;
        }

        con_lid = GAP_2_GAF_CON_LID(p_acl_conn->con_idx);
        p_stream = bap_uc_srv_get_exist_stream_by_cis_hdl(con_lid, param.cis_closed->stream->iso_handle);

        if (p_stream == NULL)
        {
            TRACE(1, "Error, no cis stream: %d 0x%x", con_lid, param.cis_closed->stream->iso_handle);
            return 0;
        }

        // Check is this iso dp already removed
        if ((p_stream->state_bf & iso_state_bf) != 0)
        {
            TRACE(1, "Patch, releasing ase, cis is disconnected before iso dp is removed");
            // Update stream state
            p_stream->state_bf &= ~(iso_state_bf);
            // Fake datapath removed event
            // 4 == BAP_UC_SRV_STREAM_EVT_DP_REMOVED
            bap_uc_srv_handle_stream_event(p_stream, 4, BT_STS_SUCCESS);
        }
    }

    return bap_uc_srv_bap_stream_event_callback(group_id, event, param);
}

void gaf_patch_init()
{
    TRACE(0, "patch:gaf_log_func_internal_patch enable");
    TRACE(0, "patch:bap_uc_srv_bap_stream_event_callback_patch enable");
    g_patch_tbl_gaf[FUNC_ID_gaf_log_func_internal] = (uint32_t)gaf_log_func_internal_patch;
    g_patch_tbl_gaf[FUNC_ID_bap_uc_srv_bap_stream_event_callback] = (uint32_t)bap_uc_srv_bap_stream_event_callback_patch;
}
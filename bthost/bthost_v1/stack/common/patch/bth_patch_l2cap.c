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
#if defined(BUILD_BTH_ROM)
#include "bth_patch_l2cap.h"

// patch table contain l2cap.c/besaud.cpp
#define BTH_PATCH_FUNC(func) [FUNC_ID_##func] = (uint32_t)func

uint32_t g_patch_tbl_l2cap[] =
{
    BTH_PATCH_FUNC(l2cap_psm_to_scid_prefix),
    BTH_PATCH_FUNC(l2cap_get_psm_target_profile_from_scid),
    BTH_PATCH_FUNC(l2cap_scid_prefix_to_converted_psm),
    BTH_PATCH_FUNC(l2cap_channel_search_scid),
    BTH_PATCH_FUNC(l2cap_start_wait_disconnect_acl_timer),
    BTH_PATCH_FUNC(l2cap_directly_disconnect_channel),
    BTH_PATCH_FUNC(l2cap_channel_close),
#ifndef BLE_ONLY_ENABLED
    BTH_PATCH_FUNC(l2cap_report_open),
    BTH_PATCH_FUNC(l2cap_send_data_auto_fragment),
#endif /* BLE_ONLY_ENABLED */
    BTH_PATCH_FUNC(l2cap_handle_signal),
    BTH_PATCH_FUNC(l2cap_handle_data),
#ifndef BLE_ONLY_ENABLED
    BTH_PATCH_FUNC(btif_besaud_callback),
    BTH_PATCH_FUNC(btif_besaud_send_cmd_no_wait),
#endif /* BLE_ONLY_ENABLED */
};

#endif
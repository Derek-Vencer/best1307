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

#if defined(__GATT_OVER_BR_EDR__)
bool SRAM_TEXT_LOC btif_is_gatt_over_br_edr_allowed_send_patch(uint8_t conidx)
{
    gap_conn_item_t *conn = NULL;
    conn = gatt_over_bredr_is_profile_connected(gap_conn_idx_as_hdl(conidx));
    if (!conn)
    {
        BGLOG_W("%s:conidx 0x%x", __func__, conidx);
        return false;
    }

    if (true == bt_callback_tws_get_ibrt_role_slave(&conn->peer_addr))
    {
        return false;
    }

    return true;
}
#endif

void gatt_patch()
{
#if defined(__GATT_OVER_BR_EDR__)
    void *p_gatt_patch;
    p_gatt_patch = (void *)btif_is_gatt_over_br_edr_allowed_send_patch;

    patch_open(PATCH_CTRL_ID_1, 0);

    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x10f33c, (uint32_t)p_gatt_patch);

    TRACE(0, "patch: enable btif_is_gatt_over_br_edr_allowed_send_patch");
#endif
}


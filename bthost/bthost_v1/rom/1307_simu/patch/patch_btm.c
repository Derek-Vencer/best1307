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
#include "bluetooth.h"
#include "bt_common.h"
#include "co_ppbuff.h"
#include "btm_i.h"
#include "hfp_i.h"
#include "a2dp_i.h"
#include "l2cap_i.h"
#include "hci_i.h"
#include "me_api.h"
#include "nvrecord_bt.h"
#include "bt_callback_func.h"
#include "bth_patch.h"

extern uint8_t btm_conn_count_mobile_links(void);


/* earbud event debounce timer define */
#define DELAY_PAGE_INTERVAL_MS     1100
static osTimerId delay_page_timer = NULL;
static void delay_start_page_callback(void const *param);
osTimerDef(DELAY_PAGE_TIMER, delay_start_page_callback);

static struct bdaddr_t target_addr;


int8  btm_conn_acl_req_patch(struct bdaddr_t *remote_bdaddr)
{
    if (btm_conn_count_mobile_links()) {
        memcpy(&target_addr, remote_bdaddr, 6);
        osTimerStart(delay_page_timer, DELAY_PAGE_INTERVAL_MS);
        TRACE(0, "delay-patch: delay %dms to page", DELAY_PAGE_INTERVAL_MS);
        return SUCCESS;
    }

    return btm_conn_acl_req(remote_bdaddr);
}

static void delay_start_page_callback(void const *param)
{
    TRACE(0, "delay-patch: timeout start page");
    bt_defer_call_func_1(btm_conn_acl_req,
                         bt_alloc_param(&target_addr));
}


void btm_patch_init()
{
    if (delay_page_timer == NULL)
        delay_page_timer = osTimerCreate(osTimer(DELAY_PAGE_TIMER), osTimerOnce, NULL);

    g_patch_tbl_hci[FUNC_ID_btm_conn_acl_req] = (uint32_t)btm_conn_acl_req_patch;
}
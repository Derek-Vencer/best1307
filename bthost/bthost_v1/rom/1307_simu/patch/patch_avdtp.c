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
#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "analog.h"
#include "bt_drv.h"
#include "bluetooth.h"
#include "me_api.h"
#include "bt_drv_interface.h"
#include "nvrecord_bt.h"
#include "nvrecord_env.h"
#include "me_api.h"
#include "a2dp_i.h"
#include "avrcp_i.h"
#include "bt_drv_reg_op.h"
#include "besbt.h"
#include "cqueue.h"
#include "btapp.h"
#include "app_bt.h"
#include "apps.h"
#include "tgt_hardware.h"
#include "a2dp_api.h"
#include "avrcp_api.h"
#include "app_a2dp.h"
#include "l2cap_api.h"
#include "bth_patch.h"
#include "patch.h"
#include "hal_location.h"

#ifndef BLE_ONLY_ENABLED

#define AVDTP_CUSTOM_CMD_VIVO 0x30

extern struct a2dp_control_t *call_a2dp_get_control_from_id(uint8_t device_id);

#define AVDTP_GET_CAP_START_RESERVED_TRANS_ID (11)

static uint8 avdtp_generate_trans_id_per_channel(struct avdtp_control_t *avdtp_ctl)
{
    uint8 curr_trans_id = avdtp_ctl->avdtp_trans_id;
    uint8 next_trans_id = (avdtp_ctl->avdtp_trans_id + 1) % 16;
    if (next_trans_id == AVDTP_GET_CAP_START_RESERVED_TRANS_ID)
    {
        next_trans_id = (next_trans_id + 1) % 16;
    }
    avdtp_ctl->avdtp_trans_id = next_trans_id;
    return curr_trans_id;
}

void bt_thread_func_send_avdtp_signal_message(uint16_t handle, const uint8_t *buff, uint16_t len)
{
    osapi_lock_stack();
    avdtp_send(handle, (uint8_t *)buff, len);
    osapi_unlock_stack();
    cobuf_free((unsigned char *)buff);
}

bt_status_t btif_a2dp_send_signal_message(const bt_bdaddr_t *remote, bt_a2dp_signal_msg_header_t *header, const uint8_t *data, uint16_t len)
{
    struct BT_DEVICE_T *curr_device = NULL;
    struct a2dp_control_t *a2dp_ctl = NULL;
    struct avdtp_control_t *avdtp_ctl = NULL;
    uint32_t signal_handle = 0;
    uint16_t msg_length = sizeof(bt_a2dp_signal_msg_header_t);
    uint16_t curr_len = 0;
    uint8_t *alloc_buff = NULL;

    curr_device = app_bt_get_connected_device_byaddr(remote);
    if (curr_device == NULL)
    {
        TRACE(0, "%s: not connected", __func__);
        return BT_STS_FAILED;
    }

    if (data && len)
    {
        msg_length += len;
    }

    a2dp_ctl = call_a2dp_get_control_from_id(curr_device->device_id);
    avdtp_ctl = a2dp_get_avdtp_control(a2dp_ctl);
    signal_handle = avdtp_get_signal_l2cap_handle(avdtp_ctl);

    alloc_buff = cobuf_malloc(msg_length);
    if (alloc_buff == NULL)
    {
        TRACE(0, "%s: alloc len %d failed", __func__, msg_length);
        return BT_STS_FAILED;
    }

    if (header->message_type == 0) // command
    {
        header->transaction = avdtp_generate_trans_id_per_channel(avdtp_ctl);
    }

    memcpy(alloc_buff+curr_len, header, sizeof(bt_a2dp_signal_msg_header_t));
    curr_len += sizeof(bt_a2dp_signal_msg_header_t);

    memcpy(alloc_buff+curr_len, data, len);
    curr_len += len;

    bt_defer_call_func_3(bt_thread_func_send_avdtp_signal_message,
        bt_fixed_param(signal_handle),
        bt_fixed_param(alloc_buff),
        bt_fixed_param(msg_length));

    return BT_STS_SUCCESS;
}


//patch func
uint8 avdtp_receive_command_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size);
uint8 avdtp_receive_response_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size);
void avdtp_receive_response_reject_handle_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size);
int avdtp_l2cap_callback_patch(uintptr_t connhdl, bt_l2cap_event_t event, bt_l2cap_callback_param_t param);

// extern fun in rom
extern uint8 avdtp_receive_discover_cmd(struct avdtp_control_t *avdtp_ctl, struct avdtp_header *req);
extern uint8 avdtp_receive_getcap_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern uint8 avdtp_receive_getallcaps_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern uint8 avdtp_receive_setconf_cmd(struct avdtp_control_t *avdtp_ctl, struct setconf_req *req, int size);
extern uint8 avdtp_receive_open_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern uint8 avdtp_receive_start_cmd(struct avdtp_control_t *avdtp_ctl, struct start_req *req, int size);
extern uint8 avdtp_receive_close_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern int8 avdtp_receive_suspend_cmd(struct avdtp_control_t *avdtp_ctl, struct suspend_req *req, int size);
extern uint8 avdtp_receive_abort_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern uint8 avdtp_receive_reconf_cmd(struct avdtp_control_t *avdtp_ctl, struct reconf_req *req, int size);
extern uint8 avdtp_receive_getconf_cmd(struct avdtp_control_t *avdtp_ctl, struct seid_req *req, int size);
extern uint8 avdtp_send_reject_resp(struct avdtp_control_t *avdtp_ctl, uint8 transaction_n, uint8 signal_id, uint8 err);
extern uint8 avdtp_receive_security_control_cmd(struct avdtp_control_t *avdtp_ctl, struct security_control_req *req, int size);
extern int8 avdtp_send_accept_resp(struct avdtp_control_t *avdtp_ctl, uint8 transaction_n, uint8 signal_id);
extern uint8 avdtp_receive_unknown_cmd(struct avdtp_control_t *avdtp_ctl, struct avdtp_header *req);
extern uint8 avdtp_receive_discover_resp(struct avdtp_control_t *avdtp_ctl, struct discover_resp *resp, uint32 size);
extern uint8 avdtp_receive_getCaps_resp(struct avdtp_control_t *avdtp_ctl, struct getcap_resp *resp, uint32 size);
extern void avdtp_set_state(struct avdtp_control_t *avdtp_ctl, avdtp_state_t state);
extern uint8 avdtp_send_discover_resp(struct avdtp_control_t *avdtp_ctl, uint8 transaction);
extern uint8 avdtp_receive_security_control_resp(struct avdtp_control_t *avdtp_ctl, struct security_control_resp *resp, uint32 size);
extern uint8 avdtp_receive_open_resp(struct avdtp_control_t *avdtp_ctl);
extern struct avdtp_remote_sep* select_lower_priority_codec_than(struct a2dp_control_t *a2dp_ctl, struct avdtp_remote_sep* rsep_list, int prio);
extern uint8 a2dp_select_capabilities(struct a2dp_control_t *a2dp_ctl, struct avdtp_remote_sep *rsep);
extern int avdtp_handle_remote_request(uint8_t device_id, bt_l2cap_accept_t *conn_req);
extern void avdtp_signal_channel_created(uint8 device_id, struct avdtp_control_t *avdtp_ctl, enum avdtp_event_enum event);
extern void avdtp_free_remote_sep_list(struct avdtp_control_t *avdtp_ctl);
extern void avdtp_delay_cmd_res_cancel(void);
struct avdtp_control_t *call_a2dp_search_avdtp_ctl_from_id(uint8_t device_id, bool is_channel_req_context);

static void avdtp_codec_setconfig_done(uint8 device_id, struct avdtp_control_t *avdtp_ctl)
{
    /* report avdtp signal channel opend when set configured */
    uint8 event = avdtp_ctl->initiator ? AVDTP_OPENED : AVDTP_ACCEPT_OPENED;
    avdtp_ctl->notify_callback(device_id, avdtp_ctl, event, avdtp_ctl->signal_session.l2cap_handle, &avdtp_ctl->remote, 0);

    if (avdtp_ctl->cfm && avdtp_ctl->cfm->setconf_done_cb)
    {
        avdtp_ctl->cfm->setconf_done_cb(avdtp_ctl);
    }
}

static struct avdtp_control_t* avdtp_search_control_from_id(uint8_t device_id)
{
    return call_a2dp_search_avdtp_ctl_from_id(device_id, false);
}

static struct avdtp_control_t* avdtp_search_control_from_handle(uint8_t device_id, uint32 l2cap_handle)
{
    struct avdtp_control_t* avdtp_ctl = NULL;

    avdtp_ctl = avdtp_search_control_from_id(device_id);
    if (!avdtp_ctl)
    {
        DEBUG_INFO(3,"(d%x) %s: cannot find avdtp from device id %x", device_id, __func__, device_id);
        return NULL;
    }

    if (avdtp_ctl->signal_session.l2cap_handle != l2cap_handle && avdtp_ctl->media_session.l2cap_handle != l2cap_handle)
    {
        DEBUG_INFO(5,"(d%x) %s: avdtp l2cap_handle mismatch sig%02x med%02x %02x", device_id, __func__,
            avdtp_ctl->signal_session.l2cap_handle, avdtp_ctl->media_session.l2cap_handle, l2cap_handle);
    }

    return avdtp_ctl;
}

struct avdtp_session *avdtp_session_search_l2caphandle(struct avdtp_control_t *avdtp_ctl, uint32 l2cap_handle)
{
    if (avdtp_ctl == NULL)
    {
        DEBUG_INFO(2,"%s: avdtp_ctl is NULL, l2cap_handle %02x", __func__, l2cap_handle);
        return NULL;
    }

    if (avdtp_ctl->signal_session.l2cap_handle == l2cap_handle)
    {
        return &avdtp_ctl->signal_session;
    }

    if (avdtp_ctl->media_session.l2cap_handle == l2cap_handle)
    {
        return &avdtp_ctl->media_session;
    }

    DEBUG_INFO(2,"%s: cannot find l2cap_handle %02x", __func__, l2cap_handle);
    return NULL;
}

uint8 avdtp_receive_command_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size)
{
    uint8 ret = 0;
    avdtp_ctl->cur_op = (void *)header;
    switch (header->signal_id)
    {
        case AVDTP_DISCOVER:
            TRACE(1,"(d%x) Received DISCOVER_CMD", device_id);
            return avdtp_receive_discover_cmd(avdtp_ctl, (void *)header);
        case AVDTP_GET_CAPABILITIES:
            TRACE(1,"(d%x) Received  GET_CAPABILITIES_CMD", device_id);
            return avdtp_receive_getcap_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_GET_ALL_CAPABILITIES: //add for match AVDTP spec version 1.3
            TRACE(1,"(d%x) Received  GET_ALL_CAPABILITIES_CMD", device_id);
            return avdtp_receive_getallcaps_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_SET_CONFIGURATION:
            TRACE(3,"(d%x) Received SET_CONFIGURATION_CMD seid %d rmid %d",
                device_id, ((struct setconf_req *)header)->acp_seid, ((struct setconf_req *)header)->int_seid);
            ret = avdtp_receive_setconf_cmd(avdtp_ctl, (void *)header, size);
            if (avdtp_ctl->state == AVDTP_STATE_CONFIGURED) {
                avdtp_codec_setconfig_done(device_id, avdtp_ctl);
            }
            return ret;
        case AVDTP_OPEN:
            TRACE(1,"(d%x) Received OPEN_CMD", device_id);
            return avdtp_receive_open_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_START:
            TRACE(1,"(d%x) Received START_CMD", device_id);
            return avdtp_receive_start_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_CLOSE:
            TRACE(1,"(d%x) Received CLOSE_CMD", device_id);
            return avdtp_receive_close_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_SUSPEND:
            TRACE(1,"(d%x) Received SUSPEND_CMD", device_id);
            return avdtp_receive_suspend_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_ABORT:
            TRACE(1,"(d%x) Received ABORT_CMD", device_id);
            return avdtp_receive_abort_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_RECONFIGURE:
            TRACE(1,"(d%x) Received RECONFIGURE_CMD", device_id);
            return avdtp_receive_reconf_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_GET_CONFIGURATION:
            TRACE(1,"(d%x) Received GET_CONFIGURATION_CMD", device_id);
            return avdtp_receive_getconf_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_SECURITY_CONTROL:
            TRACE(1,"(d%x) Received SECURITY_CONTROL_CMD", device_id);
             struct a2dp_control_t * stream = (struct a2dp_control_t * )avdtp_ctl->a2dp_stream;
            if(stream==NULL)
            {
               return avdtp_send_reject_resp(avdtp_ctl, header->transaction, header->signal_id, AVDTP_BAD_STATE);
            }
            return avdtp_receive_security_control_cmd(avdtp_ctl, (void *)header, size);
        case AVDTP_DELAYREPORT:
            TRACE(1,"(d%x) Received DELAYREPORT_CMD", device_id);
            if (avdtp_ctl->is_source) {
                return avdtp_send_accept_resp(avdtp_ctl, header->transaction, AVDTP_DELAYREPORT);
            } else {
                return avdtp_send_reject_resp(avdtp_ctl, header->transaction, header->signal_id, AVDTP_NOT_SUPPORTED_COMMAND);
            }
        case AVDTP_CUSTOM_CMD_VIVO:
            {
                bt_a2dp_custom_cmd_req_param_t param;
                param.trans_lable = header->transaction;
                param.cmd_id = header->signal_id;
                param.data_len = size - 2;
                param.cmd_data = ((uint8_t *)header) + 2;
                btif_report_bt_event(&avdtp_ctl->remote, BT_EVENT_A2DP_CUSTOM_CMD_REQ, &param);
            }
            return 0;
        default:
            TRACE(2,"(d%x) Received unknown request: %u", device_id, header->signal_id);
            return avdtp_receive_unknown_cmd(avdtp_ctl, (void *)header); //need update for 1.3
    }
}

uint8 avdtp_receive_response_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size)
{
    switch (header->signal_id)
    {
        case AVDTP_DISCOVER:
            TRACE(1,"(d%x) Receive DISCOVER_RSP", device_id);
            return avdtp_receive_discover_resp(avdtp_ctl, (void *)header, size);
        case AVDTP_GET_CAPABILITIES:
        case AVDTP_GET_ALL_CAPABILITIES:
            TRACE(1,"(d%x) Receive GET_CAPABILITIES_RSP", device_id);
            return avdtp_receive_getCaps_resp(avdtp_ctl, (void *)header, size);
        case AVDTP_SET_CONFIGURATION:
            TRACE(1,"(d%x) Receive SET_CONFIGURATION_RSP", device_id);
            avdtp_set_state(avdtp_ctl, AVDTP_STATE_CONFIGURED);
            if (avdtp_ctl->delay_resp_discover_req)
            {
                DEBUG_INFO(0,"send delayed avdtp discover resp\n");
                avdtp_send_discover_resp(avdtp_ctl, avdtp_ctl->discover_req_transaction);
            }
            avdtp_codec_setconfig_done(device_id, avdtp_ctl);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->setconf_rsp_cb)
            {
                avdtp_ctl->cfm->setconf_rsp_cb(avdtp_ctl);
            }
            break;
        case AVDTP_SECURITY_CONTROL:
            TRACE(1,"(d%x) Receive SECURITY_CONTROL_RSP", device_id);
            return avdtp_receive_security_control_resp(avdtp_ctl, (void *)header, size);
        case AVDTP_OPEN:
            TRACE(1,"(d%x) Receive OPEN_RSP", device_id);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->open_rsp_cb)
            {
                avdtp_ctl->cfm->open_rsp_cb(avdtp_ctl);
            }
            return avdtp_receive_open_resp(avdtp_ctl);
        case AVDTP_CLOSE:
            TRACE(1,"(d%x) Receive CLOSE_RSP", device_id);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->close_rsp_cb)
                avdtp_ctl->cfm->close_rsp_cb(avdtp_ctl);
            break;
        case AVDTP_SUSPEND:
            TRACE(1,"(d%x) Receive SUSPEND_RSP", device_id);
            avdtp_set_state(avdtp_ctl, AVDTP_STATE_OPEN);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->suspend_rsp_cb)
                avdtp_ctl->cfm->suspend_rsp_cb(avdtp_ctl);
            break;
        case AVDTP_START:
            TRACE(1,"(d%x) Receive START_RSP", device_id);
            avdtp_set_state(avdtp_ctl, AVDTP_STATE_STREAMING);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->start_rsp_cb)
                avdtp_ctl->cfm->start_rsp_cb(avdtp_ctl);
            break;
        case AVDTP_ABORT:
            TRACE(1,"(d%x) Receive ABORT_RSP", device_id);
            avdtp_set_state(avdtp_ctl, AVDTP_STATE_IDLE);
            if (avdtp_ctl->cfm && avdtp_ctl->cfm->abort_rsp_cb)
                avdtp_ctl->cfm->abort_rsp_cb(avdtp_ctl);
            break;
        case AVDTP_DELAYREPORT:
            TRACE(1,"(d%x) Receive DELAYREPORT_RSP", device_id);
            break;
        case AVDTP_CUSTOM_CMD_VIVO:
            {
                bt_a2dp_custom_cmd_rsp_param_t param;
                param.accepted = true;
                param.cmd_id = header->signal_id;
                param.data_len = size - 2;
                param.cmd_data = ((uint8_t *)header) + 2;
                btif_report_bt_event(&avdtp_ctl->remote, BT_EVENT_A2DP_CUSTOM_CMD_RSP, &param);
            }
            break;
        default:
            TRACE(2,"(d%x) Receive Unknown Response: %u", device_id, header->signal_id);
            break;
    }
    return FAILURE;
}

void avdtp_receive_response_reject_handle_patch(uint8 device_id, struct avdtp_control_t *avdtp_ctl, struct avdtp_header *header, uint32 size)
{
    struct avdtp_remote_sep *rsep = NULL;
    struct a2dp_control_t *a2dp_ctl = (struct a2dp_control_t*)avdtp_ctl->a2dp_stream;
    switch (header->signal_id)
    {
        case AVDTP_DISCOVER:
           break;
        case AVDTP_GET_CAPABILITIES:
        case AVDTP_GET_ALL_CAPABILITIES:
           break;
        case AVDTP_SET_CONFIGURATION:
           TRACE(2,"(d%x) Receive set configuration Response reject,size:%d", device_id, size);
           if(a2dp_ctl)
           {
               rsep = select_lower_priority_codec_than(a2dp_ctl, avdtp_ctl->rsep_list, a2dp_ctl->curr_lsep_prio);
               while (a2dp_select_capabilities(a2dp_ctl, rsep) != SUCCESS)
               {
                   rsep = select_lower_priority_codec_than(a2dp_ctl, avdtp_ctl->rsep_list, a2dp_ctl->curr_lsep_prio);
                   if (rsep == NULL) {
                       TRACE(0,"Unable to select remote SEP capabilities");
                       goto handle_fail;
                   }
               }
           }
           break;
        case AVDTP_SECURITY_CONTROL:
           break;
        case AVDTP_OPEN:
           break;
        case AVDTP_CLOSE:
           break;
        case AVDTP_SUSPEND:
           break;
        case AVDTP_START:
           break;
        case AVDTP_ABORT:
           break;
        case AVDTP_DELAYREPORT:
           break;
        case AVDTP_CUSTOM_CMD_VIVO:
            {
                bt_a2dp_custom_cmd_rsp_param_t param;
                param.accepted = false;
                param.cmd_id = header->signal_id;
                param.data_len = size - 2;
                param.cmd_data = ((uint8_t *)header) + 2;
                btif_report_bt_event(&avdtp_ctl->remote, BT_EVENT_A2DP_CUSTOM_CMD_RSP, &param);
            }
            break;
        default:
           TRACE(2,"(d%x) Receive Unknown Response: %u", device_id, header->signal_id);
           break;
    }
    return;
handle_fail:
    avdtp_close_i(&a2dp_ctl->avdtp);
}

int avdtp_l2cap_callback_patch(uintptr_t connhdl, bt_l2cap_event_t event, bt_l2cap_callback_param_t param)
{
    struct avdtp_control_t *avdtp_ctl = NULL;
    struct avdtp_session * session = NULL;
    bt_l2cap_param_t *l2cap = param.param_ptr;

    static bool signal_channel_disc_while_connecting_media_channel = false;

    if (event == BT_L2CAP_EVENT_ACCEPT)
    {
        bt_l2cap_accept_t *accept = param.accept;
        return call_wrapper(btprf, avdtp_handle_remote_request, accept->device_id, accept);
    }

    avdtp_ctl = avdtp_search_control_from_handle(l2cap->device_id, l2cap->l2cap_handle);
    if (!avdtp_ctl)
    {
        DEBUG_INFO(3, "(d%x) %s l2cap_handle %02x avdtp NULL", l2cap->device_id, __func__, l2cap->l2cap_handle);
        return 0;
    }

    switch (event)
    {
        case BT_L2CAP_EVENT_OPENED:
        {
            if (signal_channel_disc_while_connecting_media_channel) {
                l2cap_close(l2cap->l2cap_handle);
                avdtp_ctl->media_session.state = AVDTP_SESSION_CLOSE;
                avdtp_ctl->media_session.l2cap_handle = 0;
                signal_channel_disc_while_connecting_media_channel = false;
                TRACE(0, "dbg_log: disconnect the media l2cap channel due to signal channel already disconnect");
                break;
            }

            if (avdtp_ctl->signal_session.state < AVDTP_SESSION_CONNECTED || l2cap_get_converted_psm_from_handle(l2cap->l2cap_handle) == PSM_AVDTP_SIGNAL)
            {
                DEBUG_INFO(4,"(d%x) avdtp_notify: %d signal_opened initator %d l2cap_handle: %d",
                    l2cap->device_id, __LINE__, avdtp_ctl->initiator, l2cap->l2cap_handle);

                avdtp_ctl->signal_session.state = AVDTP_SESSION_CONNECTED;
                avdtp_ctl->signal_session.l2cap_handle = l2cap->l2cap_handle;
                call_wrapper(btprf, avdtp_signal_channel_created, l2cap->device_id, avdtp_ctl, l2cap->is_initiator?AVDTP_OPENED:AVDTP_ACCEPT_OPENED);
            }
            else if(avdtp_ctl->signal_session.state == AVDTP_SESSION_OPEN && avdtp_ctl->media_session.state == AVDTP_SESSION_OPEN)
            {
                DEBUG_INFO(2,"(d%x)avdtp_notify:signal_opened initator %d avdtp_already open wrong l2cap_handle: %d",
                          l2cap->device_id, avdtp_ctl->initiator, l2cap->l2cap_handle);
            }
            else
            {
                DEBUG_INFO(4,"(d%x) avdtp_notify: %d media_opened initator %d l2cap_handle: %d",
                        l2cap->device_id, __LINE__, avdtp_ctl->initiator, l2cap->l2cap_handle);

                avdtp_ctl->media_session.state = AVDTP_SESSION_OPEN;
                avdtp_ctl->notify_callback(l2cap->device_id, avdtp_ctl, AVDTP_MEDIA_OPENED, l2cap->l2cap_handle, &l2cap->peer_addr, l2cap->error_code);
            }
            avdtp_ctl->pending_avdtp_signal_data_count = 0;
            break;
        }
        case BT_L2CAP_EVENT_CLOSED:
        {
            DEBUG_INFO(4,"(d%x) avdtp_notify: event %d handle %02x",
                    l2cap->device_id, event, l2cap->l2cap_handle);

            session = avdtp_session_search_l2caphandle(avdtp_ctl, l2cap->l2cap_handle);
            if (session == NULL)
            {
                DEBUG_INFO(1,"(d%x) avdtp_notify: L2CAP_CHANNEL_CLOSED (NULL)", l2cap->device_id);
                break;
            }

            gap_acl_rx_slowdown_stop(HCI_CONN_TYPE_BT_ACL, BT_ADDR_TYPE_PUBLIC, &avdtp_ctl->remote);

            avdtp_ctl->pending_avdtp_signal_data_count = 0;
            avdtp_ctl->same_sepid_workaround_for_redmi5 = 0;
            avdtp_ctl->notify_callback(l2cap->device_id, avdtp_ctl, AVDTP_CLOSED, l2cap->l2cap_handle, &l2cap->peer_addr, l2cap->error_code);

            if (session == &avdtp_ctl->signal_session)
            {
                avdtp_ctl->signal_session.state = AVDTP_SESSION_CLOSE;
                avdtp_ctl->signal_session.l2cap_handle = 0;

                if (avdtp_ctl->media_session.state == AVDTP_SESSION_CONNECTING)
                {
                    TRACE(0, "dbg_log: set signal_channel_disc_while_connecting_media_channel");
                    signal_channel_disc_while_connecting_media_channel = true;
                }
            }
            else
            {
                avdtp_ctl->media_session.state = AVDTP_SESSION_CLOSE;
                avdtp_ctl->media_session.l2cap_handle = 0;
            }

            if (avdtp_ctl->signal_session.state == AVDTP_SESSION_CLOSE &&
                avdtp_ctl->media_session.state == AVDTP_SESSION_CLOSE)
            {
                DEBUG_INFO(1,"(d%x) avdtp_notify: both signal and media session closed", l2cap->device_id);
                avdtp_set_state(avdtp_ctl, AVDTP_STATE_IDLE);

                if (l2cap->error_code != BT_ECODE_IBRT_SLAVE_CLEANUP)
                {
                    /* there is a corner case when phone re-config and re-create media channel,
                       and the user click disconnect link very quickly, it has a chance that the
                       media channel is not established completely stucked in the config waiting state.
                       If we dont close this channel, not all l2cap channel are closed, then the
                       acl link may not be freed if remote dont free it.
                    */
                    l2cap_find_and_free_pending_avdtp_channel(&avdtp_ctl->remote);
                }

                memset(&avdtp_ctl->remote, 0, sizeof(avdtp_ctl->remote));
                avdtp_ctl->initiator = 0;
                avdtp_ctl->avdtp_trans_id = 0;

                //free remote_sep_list
                avdtp_free_remote_sep_list(avdtp_ctl);
            }

#if defined(IBRT)
            avdtp_delay_cmd_res_cancel();
#endif
            break;
        }
        case BT_L2CAP_EVENT_TX_DONE:
        {
            if (avdtp_ctl->pending_avdtp_signal_data_count)
            {
                avdtp_ctl->pending_avdtp_signal_data_count -= 1;
                if (avdtp_ctl->pending_avdtp_signal_data_count == 0)
                {
                    gap_acl_rx_slowdown_stop(HCI_CONN_TYPE_BT_ACL, BT_ADDR_TYPE_PUBLIC, &avdtp_ctl->remote);
                }
            }
            if (avdtp_ctl->state == AVDTP_STATE_OPEN || avdtp_ctl->state == AVDTP_STATE_STREAMING)
            {
                avdtp_ctl->notify_callback(l2cap->device_id, avdtp_ctl, AVDTP_TX_HANDLED, l2cap->l2cap_handle, l2cap->tx_priv_rx_ppb, 0);
            }
            break;
        }
        case BT_L2CAP_EVENT_RX_DATA:
        {
            struct avdtp_header *header = NULL;
            struct pp_buff *ppb = l2cap->tx_priv_rx_ppb;

            header = (struct avdtp_header *)ppb_get_data(ppb);

            session = avdtp_session_search_l2caphandle(avdtp_ctl, l2cap->l2cap_handle);
            if (session == NULL)
            {
                TRACE(1,"(d%x) avdtp_datarecv: no session to recv data", l2cap->device_id);
                return 0;
            }

            if (session->state == AVDTP_SESSION_CLOSE)
            {
                TRACE(1,"(d%x) avdtp_datarecv: wrong session to recv data", l2cap->device_id);
                return 0;
            }

            if (session == &avdtp_ctl->signal_session)
            {
                if (header->message_type == AVDTP_MSG_TYPE_COMMAND)
                {
                    avdtp_receive_command_patch(l2cap->device_id, avdtp_ctl, header, ppb->len);
                    // call_wrapper(btprf, avdtp_receive_command, l2cap->device_id, avdtp_ctl, header, ppb->len);
                }
                else if (header->message_type == AVDTP_MSG_TYPE_ACCEPT)
                {
                    avdtp_receive_response_patch(l2cap->device_id, avdtp_ctl, header, ppb->len);
                    // call_wrapper(btprf, avdtp_receive_response, l2cap->device_id, avdtp_ctl, header, ppb->len);
                }
                else if(header->message_type == AVDTP_MSG_TYPE_REJECT ||
                        header->message_type == AVDTP_MSG_TYPE_GENERAL_REJECT)
                {
                    avdtp_receive_response_reject_handle_patch(l2cap->device_id, avdtp_ctl, header, ppb->len);
                    // call_wrapper(btprf, avdtp_receive_response_reject_handle, l2cap->device_id, avdtp_ctl, header, ppb->len);
                }
                else
                {
                    TRACE(1,"(d%x) avdtp_datarecv: unsupported message type", l2cap->device_id);
                }
            }
            else
            {
                avdtp_ctl->datarecv_callback(l2cap->device_id, avdtp_ctl, l2cap->l2cap_handle, ppb);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

uint8 SRAM_TEXT_LOC avdtp_send_discover_resp_patch(struct avdtp_control_t *avdtp_ctl, uint8 transaction)
{
    struct avdtp_header *resp;
    struct a2dp_control_t *a2dp_ctl = NULL;
    struct avdtp_local_sep *lsep = NULL;
    uint8 *temp_ptr;
    uint16 resp_size = 2; //header
    int8 ret = FAILURE;

    a2dp_ctl = (struct a2dp_control_t*)avdtp_ctl->a2dp_stream;
    lsep = a2dp_ctl->lsep_list;

    while (lsep)
    {
        if (lsep->allocated && lsep->sep_is_discoverable)
        {
            resp_size += 2;
        }
        lsep = lsep->next;
    }

    TRACE(3,"%s %d lsep count %d", __func__, __LINE__, (resp_size-2)/2);

    resp = (struct avdtp_header *)cobuf_malloc(resp_size);
    if (NULL == resp)
    {
        TRACE(0,"No enough buffer");
        return FAILURE;
    }

    resp->message_type = AVDTP_MSG_TYPE_ACCEPT;
    resp->packet_type = AVDTP_PKT_TYPE_SINGLE;
    resp->transaction = transaction;
    resp->signal_id = AVDTP_DISCOVER;
    resp->rfa0 = 0; /* clear rfa bits */

    temp_ptr = (uint8 *)resp + 2;

    lsep = a2dp_ctl->lsep_list;
    while (lsep)
    {
        if (lsep->allocated && lsep->sep_is_discoverable)
        {
            if(avdtp_ctl->delay_resp_discover_req &&
               lsep->info.inuse == 1)
            {
                TRACE(1,"%s reset inuse", __func__);
                temp_ptr[0] = 0;
                temp_ptr[1] = 0;
            }
            else
            {
                memcpy(temp_ptr, (void *)&lsep->info, 2);
            }
            temp_ptr += 2;
        }
        lsep = lsep->next;
    }
    avdtp_ctl->delay_resp_discover_req = false;
    ret = avdtp_send(avdtp_ctl->signal_session.l2cap_handle, (uint8 *)resp, resp_size);
    cobuf_free((void *)resp);

    return ret;
}

void avdtp_patch_init()
{
    TRACE(0, "patch:avdtp_receive_command_patch enable");
    TRACE(0, "patch:avdtp_receive_response_patch enable");
    TRACE(0, "patch:avdtp_receive_response_reject_handle_patch enable");
    TRACE(0, "patch:avdtp_l2cap_callback_patch enable");
    g_patch_tbl_btprf[FUNC_ID_avdtp_receive_command] = (uint32_t)avdtp_receive_command_patch;
    g_patch_tbl_btprf[FUNC_ID_avdtp_receive_response] = (uint32_t)avdtp_receive_response_patch;
    g_patch_tbl_btprf[FUNC_ID_avdtp_receive_response_reject_handle] = (uint32_t)avdtp_receive_response_reject_handle_patch;
    g_patch_tbl_btprf[FUNC_ID_avdtp_l2cap_callback] = (uint32_t)avdtp_l2cap_callback_patch;

    patch_open(PATCH_CTRL_ID_1, 0);
    // 0014ae84 g     F .rom_text	00000090 avdtp_send_discover_resp
    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x14ae84, (uint32_t)avdtp_send_discover_resp_patch);
    TRACE(0, "patch:avdtp_send_discover_resp_patch enable");
}

#endif /* BLE_ONLY_ENABLED */
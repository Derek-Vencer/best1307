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
#include "btm_i.h"
#include "hci_i.h"
#include "bt_if.h"
#include "stdarg.h"
#include "nvrecord_bt.h"
#include "patch.h"
#include "hal_location.h"

/* patch for avdtp */
extern void avdtp_patch_init();
extern void avrcp_patch();
extern void gaf_patch_init();
extern void gatt_patch();
extern void a2dp_patch();
extern void hfp_patch();
extern void btm_patch_init();

extern bt_status_t gap_parse_curr_adv_parameters(uint8_t adv_handle, const gap_adv_param_t *param, gap_adv_callback_t cb, gap_advertising_t *adv);

extern uint32_t g_patch_tbl_hci[];

/*PATCH 1*/
bt_status_t gap_parse_curr_adv_parameters_patch(uint8_t adv_handle, const gap_adv_param_t *param, gap_adv_callback_t cb, gap_advertising_t *adv)
{
    bt_status_t status = gap_parse_curr_adv_parameters(adv_handle, param, cb, adv);

    if (status != BT_STS_SUCCESS)
    {
        return status;
    }

    // Patch start to correct event prop
    adv->set_param.adv_event_props = param->use_legacy_pdu ? HCI_LE_USE_LEGACY_ADV_PDU : 0;

    if (param->use_legacy_pdu)
    {
        if (param->directed_adv)
        {
            adv->set_param.adv_event_props |= HCI_LE_CONN_ADVERTISING;
            adv->set_param.adv_event_props |= HCI_LE_DIRECTED_ADVERTISING;
            if (param->high_duty_directed_adv)
            {
                adv->set_param.adv_event_props |= HCI_LE_HIGH_DUTY_DIRECTED_CONN_ADV;
            }
        }
        else if (param->connectable)
        {
            adv->set_param.adv_event_props |= HCI_LE_CONN_ADVERTISING;
            adv->set_param.adv_event_props |= HCI_LE_SCAN_ADVERTISING;
        }
        else if (param->scannable)
        {
            adv->set_param.adv_event_props |= HCI_LE_SCAN_ADVERTISING;
        }
    }
    else
    {
        /**
         * if extended adv pdu types are being used then:
         *      1. the advertisement shall not be both connectable and scannable
         *      2. high duty cycle directed connectable advertising shall not be used
         */
        if (param->directed_adv)
        {
            adv->set_param.adv_event_props |= HCI_LE_DIRECTED_ADVERTISING;
            if (param->connectable)
            {
                adv->set_param.adv_event_props |= HCI_LE_CONN_ADVERTISING;
            }
            else if (param->scannable)
            {
                adv->set_param.adv_event_props |= HCI_LE_SCAN_ADVERTISING;
            }
        }
        else if (param->connectable)
        {
            adv->set_param.adv_event_props |= HCI_LE_CONN_ADVERTISING;
        }
        else if (param->scannable)
        {
            adv->set_param.adv_event_props |= HCI_LE_SCAN_ADVERTISING;
        }
        if (param->anonymous_adv)
        {
            adv->set_param.adv_event_props |= HCI_LE_OMIT_ADV_ADDRESS_FROM_ALL_PDU;
        }
        if (param->include_tx_power_data)
        {
            adv->set_param.adv_event_props |= HCI_LE_INCL_TX_POWER_AT_LEAST_ONE_ADV;
        }
    }

    return BT_STS_SUCCESS;
}

/*PATCH 2*/
static void btm_security_link_key_req_patch(struct bdaddr_t *remote)
{
    /*
     *  Reply link key or Negative reply link key
    */
    TRACE(0, "BTH ROM btm_security_link_key_req_patch");
    btif_device_record_t record;
    bt_status_t status = BT_STS_FAILED;
    uint8 linkkey[16];
    bool find_ok = false;

    status = nv_record_ddbrec_find((const bt_bdaddr_t *)remote, &record);
    DEBUG_INFO(0, "link_key_req");
    DUMP8("%02x ", record.linkKey, 16);

    if (status == BT_STS_SUCCESS)
    {
        memset(linkkey, 0, sizeof(linkkey));
        if (memcmp(linkkey, record.linkKey, 16))
        {
            find_ok = true;
        }
    }

    if (find_ok)
    {
        DEBUG_INFO(0, "Link key exist");
        DUMP8("%02x ", record.linkKey, 16);
        memcpy(linkkey, record.linkKey, 16);
        btlib_hcicmd_linkkey_reply(remote, linkkey);
    }
    else
    {
        DEBUG_INFO(0, "Link key not exist");
        btlib_hcicmd_linkkey_neg_reply(remote);
    }
}

/* PATCH 3 */
typedef struct {
    gatt_multi_notify_item_t *notify;
    uint16_t count;
    uint16_t config;
    uint32_t con_bfs;
    uint16_t num_client_notified;
} gatt_send_notify_param_t;

typedef struct {
    const gatt_attribute_t *attr;
    uint16_t attr_handle;
    uint16_t end_handle;
} gatt_item_header_t;

typedef struct gatt_character_t {
    struct gatt_character_t *next;
    gatt_item_header_t head;
    uint8_t desc_count;
    uint8_t attr_flags;
    uint16_t cccd_seqn;
    uint16_t char_value_handle;
    uint16_t char_uuid; // if zero 128 bit characteristic uuid
} gatt_character_t;

typedef struct {
    gatt_service_t *service;
    gatt_character_t *character;
} gatt_local_char_search_result_t;

typedef struct {
    uint8_t attr_flags;
    uint8_t attr_perm;
    uint16_t value_len;
    const uint8_t *value_ptr;
} gatt_attr_value_t;

extern bool gatt_search_local_character(const uint8_t *service, uint8_t service_inst_id, const uint8_t *character, uint8_t char_inst_id, gatt_local_char_search_result_t* out);
extern gatt_attr_value_t gatt_get_attr_value(const gatt_attribute_t *item);
extern bt_status_t gatt_send_value_notification(att_conn_item_t *conn, gatt_service_t *s, gatt_character_t *c, const uint8_t *value, uint16_t len);
extern bt_status_t gatt_send_multi_value_notification(att_conn_item_t *conn, const gatt_multi_notify_item_t *notify, uint16_t count);
extern bt_status_t gatt_send_value_indication(att_conn_item_t *conn, gatt_service_t *s, gatt_character_t *c, const uint8_t *value, uint16_t len);
extern void gatt_server_recv_notify_tx_done(att_conn_item_t *conn, uint8_t error_code, gatt_service_t *s, gatt_character_t *c);

bool gatt_send_notify_for_each_conn_patch(att_conn_item_t *conn, void *param)
{
    bt_status_t status = BT_STS_SUCCESS;
    gatt_send_notify_param_t *p = param;
    gatt_multi_notify_item_t *notify = NULL;
    gatt_local_char_search_result_t result = {0};
    gatt_attr_value_t decl_value = {0};

    if (p->count == 0)
    {
        return gap_end_loop;
    }

    if (p->con_bfs != GAP_ALL_CONNS)
    {
        uint32_t conn_bf = gap_conn_bf(conn->head.con_idx);
        if ((!conn->head.conn_flag.conn_ready) || ((p->con_bfs & conn_bf) == 0))
        {
            return gap_continue_loop; // continue loop next client conn
        }
    }

    notify = p->notify;

    for (; notify < p->notify + p->count; notify += 1)
    {
        if (!gatt_search_local_character(notify->service, notify->service_inst_id,
                                                notify->character, notify->char_instance_id, &result))
        {
            CO_LOG_ERR_2(BT_STS_INVALID_CHARACTER_INSTANCE,
                gatts_get_char_byte_16_bit_part_uuid(notify->character), notify->char_instance_id);
            status = BT_STS_INVALID_CHARACTER_INSTANCE;
            break;
        }
        // Check properties from character decalration
        decl_value = gatt_get_attr_value(result.character->head.attr);
        if (decl_value.value_ptr == NULL)
        {
            CO_LOG_ERR_2(BT_STS_INVALID_CHAR_TYPE,
                gatts_get_char_byte_16_bit_part_uuid(notify->character), notify->char_instance_id);
            status = BT_STS_INVALID_CHAR_TYPE;
            break;
        }

        notify->cccd_config = p->config;
        // Check properties contains ntf or ind properties
        notify->cccd_config &= ~(((decl_value.value_ptr[0] & GATT_NTF_PROP) == 0) ? GATT_CCCD_SET_NOTIFICATION : 0);
        notify->cccd_config &= ~(((decl_value.value_ptr[0] & GATT_IND_PROP) == 0) ? GATT_CCCD_SET_INDICATION : 0);
        if (notify->cccd_config == 0)
        {
            CO_LOG_ERR_3(BT_STS_REGISTER_NOT_FOUND,
                gatts_get_char_byte_16_bit_part_uuid(notify->character),
                                                notify->char_instance_id, decl_value.value_ptr[0]);
            status = BT_STS_REGISTER_NOT_FOUND;
            break;
        }

        notify->s = result.service;
        notify->c = result.character;
    }

    // This client registered all the notifications, notify these character values to the client
    if (status == BT_STS_SUCCESS)
    {
        p->num_client_notified += 1;

        if (p->count == 1)
        {
            notify = p->notify;

            if (notify->cccd_config & GATT_CCCD_SET_NOTIFICATION)
            {
                status = gatt_send_value_notification(conn, notify->s, notify->c, notify->value, notify->value_len);
            }
            else
            {
                status = gatt_send_value_indication(conn, notify->s, notify->c, notify->value, notify->value_len);
            }
        }
        else
        {
            status = gatt_send_multi_value_notification(conn, p->notify, p->count);
        }
    }

    // Any err happened and report param is not null and report first notify param back
    if (status != BT_STS_SUCCESS && p->notify->s != NULL && p->notify->c != NULL)
    {
        bt_thread_call_func_4(gatt_server_recv_notify_tx_done, bt_fixed_param(conn), bt_fixed_param(status),
                                                                bt_fixed_param(p->notify->s), bt_fixed_param(p->notify->c));
    }

    return gap_continue_loop; // continue loop next client conn
}

/**
 * Patch for wrong soft_patch_set
*/
#define IS_ADDR_NOT_IN_RANGE(addr, taddr, size)     ((addr < taddr) || (addr > (taddr + size)))
extern  bool smp_wait_remote_key_distribute(smp_conn_item_t *smp, smp_event_t event, const smp_receive_t *recv);
extern int8 btm_security_askfor_authority(uint8_t device_id, uint16 conn_handle, uint16 psm, struct l2cap_channel *channel);

int smp_wait_remote_key_distribute_patch(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
    TRACE(0, "smp_wait_remote_key_distribute_patch: caller=%p, p1=%d, p2=%d, p3=%d, p4=%d", __builtin_return_address(0),
              p1, p2, p3, p4);

    // 0015ad2c g     F .rom_text	000000f8 btm_security_askfor_authority
    if (IS_ADDR_NOT_IN_RANGE((uint32_t)__builtin_return_address(0), 0x127eec, 0xa8) && IS_ADDR_NOT_IN_RANGE((uint32_t)__builtin_return_address(0), 0x127f94, 0xa8)) {
        uint8_t device_id = (uint8_t)p1;
        uint16 conn_handle = (uint16)p2;
        uint16 psm = (uint16)p3;
        struct l2cap_channel *channel = (struct l2cap_channel *)p4;
        return btm_security_askfor_authority(device_id, conn_handle, psm, channel);
    } else {
        smp_conn_item_t *smp = (smp_conn_item_t *)p1;
        smp_event_t event = (smp_event_t)p2;
        const smp_receive_t *recv = (const smp_receive_t *)p3;
        return smp_wait_remote_key_distribute(smp, event, recv);
    }
}

void bth_cvsd_config_patch()
{
    uint8_t param[32];
    uint16 opcode = btm_get_btstack_chip_config()->hci_dbg_set_sync_config_cmd_opcode;

    param[0] = CFG_SYNC_CONFIG_PATH & 0xFF;
    param[1] = (CFG_SYNC_CONFIG_PATH >> 8) & 0xFF;
    param[2] = CFG_SYNC_CONFIG_MAX_BUFFER;
    /* CFG_SYNC_CONFIG_CVSD_BYPASS should 1 */
    param[3] = 1;

    TRACE(0, "bth_cvsd_config_patch");
    hci_send_command(opcode, param, 4);
}

int8 SRAM_TEXT_LOC btlib_hcicmd_accept_sync_conn_req_patch(struct bdaddr_t *bdaddr,
                                       uint32 tx_bandwidth, uint32 rx_bandwidth,
                                       uint16 max_latency, uint16 voice_setting,
                                       uint8 retx_effort, uint16 pkt_type)
{
    struct pp_buff *ppb = NULL;
    struct hci_cp_accept_sync_conn *param;

    /* clear the 5th bit in voice_setting */
    uint16_t pos = 0x20;
    voice_setting &= ~pos;

    ppb = hci_create_cmd_packet(HCI_ACCEPT_SYNC_CONN, sizeof(struct hci_cp_accept_sync_conn), NULL);
    if (ppb == NULL)
    {
        return FAILURE;
    }
    param = (struct hci_cp_accept_sync_conn *)ppb_put(ppb, sizeof(struct hci_cp_accept_sync_conn));

    param->bdaddr = *bdaddr;
    param->tx_bandwidth = tx_bandwidth;
    param->rx_bandwidth = rx_bandwidth;
    param->max_latency = max_latency;
    param->voice_setting = voice_setting;
    param->retx_effort = retx_effort;
    param->pkt_type = pkt_type;

    return hci_send_cmd_packet(ppb);
}

void bth_stack_patch_init(void)
{
    TRACE(0, "patch:btm_security_link_key_req_patch enable");
    TRACE(0, "patch:gap_parse_curr_adv_parameters_patch enable");
    TRACE(0, "patch:gatt_send_notify_for_each_conn_patch enable");
    g_patch_tbl_hci[FUNC_ID_btm_security_link_key_req] = (uint32_t)btm_security_link_key_req_patch;
    g_patch_tbl_basic[FUNC_ID_gap_parse_curr_adv_parameters] = (uint32_t)gap_parse_curr_adv_parameters_patch;
    g_patch_tbl_basic[FUNC_ID_gatt_send_notify_for_each_conn] = (uint32_t)gatt_send_notify_for_each_conn_patch;
    g_patch_tbl_basic[FUNC_ID_smp_wait_remote_key_distribute] = (uint32_t)smp_wait_remote_key_distribute_patch;

    // 0015b750 g     F .rom_text	00000058 btlib_hcicmd_accept_sync_conn_req
    patch_open(PATCH_CTRL_ID_1, 0);
    patch_enable(PATCH_CTRL_ID_1, PATCH_TYPE_FUNC, 0x15b750, (uint32_t)btlib_hcicmd_accept_sync_conn_req_patch);

#ifndef BLE_ONLY_ENABLED
    avrcp_patch();
    avdtp_patch_init();
    gatt_patch();
    a2dp_patch();
    hfp_patch();
    btm_patch_init();
#endif /* BLE_ONLY_ENABLED */
    gaf_patch_init();
}

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
#include "bth_patch_basic.h"

#define BTH_PATCH_FUNC(func) [FUNC_ID_##func] = (uint32_t)func

uint32_t g_patch_tbl_basic[] =
{
    BTH_PATCH_FUNC(gatt_conn_ready_handler),
    BTH_PATCH_FUNC(gap_impl_get_peer_resolved_addr),
    BTH_PATCH_FUNC(gap_get_peer_resolved_address),
    BTH_PATCH_FUNC(gap_report_connection_event),
    BTH_PATCH_FUNC(gap_recv_smp_encrypted),
    BTH_PATCH_FUNC(gap_resolving_list_check_activity),
    BTH_PATCH_FUNC(gap_enable_address_resolution),
    BTH_PATCH_FUNC(gap_filter_list_check_activity),
    BTH_PATCH_FUNC(gap_adv_conn_complete_handle),
    BTH_PATCH_FUNC(gap_impl_set_adv_parameters),
    BTH_PATCH_FUNC(gap_parse_curr_adv_parameters),
    BTH_PATCH_FUNC(gap_adv_has_same_parameters),
    BTH_PATCH_FUNC(gap_create_advertising),
    BTH_PATCH_FUNC(gap_select_adv_timing),
    BTH_PATCH_FUNC(gap_select_scan_timing),
    BTH_PATCH_FUNC(gap_impl_set_scan_params),
    BTH_PATCH_FUNC(gap_select_init_timing),
    BTH_PATCH_FUNC(gap_init_conn_complete_handle),
    BTH_PATCH_FUNC(gap_impl_le_create_conn),
    BTH_PATCH_FUNC(gap_impl_start_initiating),
    BTH_PATCH_FUNC(gap_le_conn_is_established),
    BTH_PATCH_FUNC(smp_get_local_address),
    BTH_PATCH_FUNC(smp_get_peer_address),
    BTH_PATCH_FUNC(smp_security_requirements_check),
    BTH_PATCH_FUNC(smp_prepare_for_encryption),
    BTH_PATCH_FUNC(smp_receive_peer_ltk_req),
    BTH_PATCH_FUNC(smp_reply_ltk_req),
    BTH_PATCH_FUNC(smp_start_pairing_request),
    BTH_PATCH_FUNC(smp_start_security_request),
    BTH_PATCH_FUNC(smp_select_pairing_method),
    BTH_PATCH_FUNC(smp_distribute_keys),
    BTH_PATCH_FUNC(smp_wait_remote_key_distribute),
    BTH_PATCH_FUNC(smp_start_phase_3_key_distribute),
    BTH_PATCH_FUNC(smp_phase_transfer),
    BTH_PATCH_FUNC(smp_init_sc_gen_ltk_and_encrypt),
    BTH_PATCH_FUNC(smp_resp_sc_gen_ltk_and_encrypt),
    BTH_PATCH_FUNC(smp_init_sc_numeric_compare),
    BTH_PATCH_FUNC(smp_resp_sc_numeric_compare),
    BTH_PATCH_FUNC(smp_init_sc_passkey_entry),
    BTH_PATCH_FUNC(smp_resp_sc_passkey_entry),
    BTH_PATCH_FUNC(smp_init_sc_oob_method),
    BTH_PATCH_FUNC(smp_resp_sc_oob_method),
    BTH_PATCH_FUNC(smp_init_secure_rx_handle),
    BTH_PATCH_FUNC(smp_resp_secure_rx_handle),
    BTH_PATCH_FUNC(smp_init_rx_handle),
    BTH_PATCH_FUNC(smp_resp_rx_handle),
    BTH_PATCH_FUNC(smp_receive_packet),
    BTH_PATCH_FUNC(gatt_report_mtu_changed),
    BTH_PATCH_FUNC(gatt_cache_save_handles),
    BTH_PATCH_FUNC(gatt_cache_restore_handles),
    BTH_PATCH_FUNC(gatt_cache_restore),
    BTH_PATCH_FUNC(gatt_cache_save),
    BTH_PATCH_FUNC(att_bearer_add_new),
    BTH_PATCH_FUNC(att_bearer_close),
    BTH_PATCH_FUNC(att_recv_request),
    BTH_PATCH_FUNC(att_recv_response),
    BTH_PATCH_FUNC(att_bearer_recv_req_packet),
    BTH_PATCH_FUNC(att_receive_packet),
    BTH_PATCH_FUNC(att_l2cap_callback),
    BTH_PATCH_FUNC(att_bearer_send_cmd_or_ntf),
    BTH_PATCH_FUNC(att_bearer_process_req_ind_q),
    BTH_PATCH_FUNC(gatt_server_conn_handle),
    BTH_PATCH_FUNC(gatt_conn_event_handler),
    BTH_PATCH_FUNC(gatt_add_svc_to_conn),
    BTH_PATCH_FUNC(gatt_send_notify_for_each_conn),
    BTH_PATCH_FUNC(gatt_server_recv_read_req),
    BTH_PATCH_FUNC(gatt_server_recv_write_req),
    BTH_PATCH_FUNC(gatt_for_each_attr_callback),
    BTH_PATCH_FUNC(gatt_server_check_notify_permission),
};

uint32_t *bth_get_basic_patch_table(void)
{
    return g_patch_tbl_basic;
}
#endif

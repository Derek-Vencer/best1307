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
#include "bth_patch_hci.h"

// patch table contain hci.c/btm.c/me_api.c
#define BTH_PATCH_FUNC(func) [FUNC_ID_##func] = (uint32_t)func

uint32_t g_patch_tbl_hci[] =
{
    BTH_PATCH_FUNC(hci_handle_fc_on_disconnect),
    BTH_PATCH_FUNC(hci_handle_conn_event),
    BTH_PATCH_FUNC(hci_cmd_start_handle),
    BTH_PATCH_FUNC(hci_rx_handle),
    BTH_PATCH_FUNC(hci_send_all_pending_host_num_complete),
    BTH_PATCH_FUNC(hci_rx_num_completed_packet_inc),
    BTH_PATCH_FUNC(hci_create_cmd_packet_with_priv),
    BTH_PATCH_FUNC(hci_cmd_wait_complete_timeout),
    BTH_PATCH_FUNC(hci_free_cmd_packet),
    BTH_PATCH_FUNC(hci_cmd_end_handle),
    BTH_PATCH_FUNC(hci_send_pending_cmd_packet),
    BTH_PATCH_FUNC(hci_impl_send_cmd_packet),
    BTH_PATCH_FUNC(hci_send_command_with_callback),
    BTH_PATCH_FUNC(hci_simulate_event),
    BTH_PATCH_FUNC(hci_send_bt_acl_packet),
    BTH_PATCH_FUNC(hci_clean_rx_packet),
    BTH_PATCH_FUNC(hci_clean_tx_packet),
    BTH_PATCH_FUNC(hci_clean_cmd_packet),
    BTH_PATCH_FUNC(hci_get_iso_rx_packet),
    BTH_PATCH_FUNC(hci_send_iso_packet),
    BTH_PATCH_FUNC(hci_tx_iso_packet_handle),
    BTH_PATCH_FUNC(hci_tx_iso_ind),
    BTH_PATCH_FUNC(hci_handle_iso_tx_num_of_complete),
    BTH_PATCH_FUNC(hci_fc_each_link_has_tx_chance),
    BTH_PATCH_FUNC(hci_tx_buff_process),
#ifndef BLE_ONLY_ENABLED
    BTH_PATCH_FUNC(btm_conn_delete_free),
    BTH_PATCH_FUNC(btm_conn_add_new),
    BTH_PATCH_FUNC(btm_chip_init_continue),
    BTH_PATCH_FUNC(btm_chip_init_continue_noraml_test_mode_switch),
    BTH_PATCH_FUNC(btm_conn_acl_req),
    BTH_PATCH_FUNC(btm_conn_sco_req),
    BTH_PATCH_FUNC(btm_conn_acl_close),
    BTH_PATCH_FUNC(btm_conn_clear_rfcomm_credit_ignore_tx_flag),
    BTH_PATCH_FUNC(btm_conn_set_rfcomm_credit_ignore_tx_flag),
    BTH_PATCH_FUNC(btm_set_audio_default_param),
    BTH_PATCH_FUNC(btm_ibrt_report_snoop_acl_disconnected),
    BTH_PATCH_FUNC(btm_save_ctx),
    BTH_PATCH_FUNC(btm_restore_ctx),
    BTH_PATCH_FUNC(btm_sync_conn_audio_connected),
    BTH_PATCH_FUNC(btm_security_link_key_notify),
    BTH_PATCH_FUNC(btm_security_link_key_req),
    BTH_PATCH_FUNC(btm_security_authen_complete),
    BTH_PATCH_FUNC(btm_security_encryption_change),
    BTH_PATCH_FUNC(btm_security_askfor_authority),
    BTH_PATCH_FUNC(btm_create_acl_connection_fail_process),
    BTH_PATCH_FUNC(btm_accept_sync_conn_internal),
    BTH_PATCH_FUNC(btm_conn_accept_extra_acl_req_check),
    BTH_PATCH_FUNC(btm_conn_handle_extra_acl_conn_req),
    BTH_PATCH_FUNC(btm_conn_disconnect_process),
    BTH_PATCH_FUNC(btm_process_init_state_cmd_cmpl),
    BTH_PATCH_FUNC(btm_process_cmd_complete_evt),
    BTH_PATCH_FUNC(btm_get_bdaddr_from_pending_hci_cmd),
    BTH_PATCH_FUNC(btif_me_reconnect_next_device),
    BTH_PATCH_FUNC(btif_me_wait_acl_complete),
    BTH_PATCH_FUNC(btif_me_wait_acl_complete_callback),
    BTH_PATCH_FUNC(btif_me_event_report),
    BTH_PATCH_FUNC(btif_me_source_event_report),
    BTH_PATCH_FUNC(btif_me_is_creating_source_link),
    BTH_PATCH_FUNC(btif_me_is_accepting_source_link),
    BTH_PATCH_FUNC(me_event_report_handler),
    BTH_PATCH_FUNC(btif_me_set_accessible_mode),
    BTH_PATCH_FUNC(btif_me_save_record_ctx),
    BTH_PATCH_FUNC(btif_me_set_record_ctx),
#endif /* BLE_ONLY_ENABLED */
};
#endif

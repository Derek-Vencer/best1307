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

#include "bth_patch_gaf.h"

#define BTH_PATCH_FUNC(func) [FUNC_ID_##func] = (uint32_t)func

uint32_t g_patch_tbl_gaf[FUNC_ID_GAF_PATCH_NUM_MAX] =
{
    BTH_PATCH_FUNC(gaf_prf_alloc_or_get_con_lid_with_gap_conidx),
#if (ACC_MCS_ENABLE)
    BTH_PATCH_FUNC(mcs_prepare_service_attr_list),
#endif
#if (ACC_TBS_ENABLE)
    BTH_PATCH_FUNC(tbs_prepare_service_attr_list),
#endif
#if (ACC_OTS_ENABLE)
    BTH_PATCH_FUNC(ots_prepare_service_attr_list),
#endif
    BTH_PATCH_FUNC(csis_prepare_service_attr_list),
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_FUNC(ascs_send_notify),
#endif
    BTH_PATCH_FUNC(asc_common_ase_state_changes_check),
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_FUNC(ascc_write_ase_cp),
#endif
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_FUNC(bap_uc_srv_before_set_ase_state),
    BTH_PATCH_FUNC(bap_uc_srv_queued_disconnect_cis),
    BTH_PATCH_FUNC(bap_uc_srv_iso_cb_cis_conn_request),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_FUNC(bap_uc_cli_before_set_ase_state),
    BTH_PATCH_FUNC(bap_uc_cli_prepare_cis_hdl_to_create_cis),
    BTH_PATCH_FUNC(bap_uc_cli_queued_create_cis),
    BTH_PATCH_FUNC(bap_uc_cli_queued_disconnect_cis),
    BTH_PATCH_FUNC(bap_uc_cli_loop_prepare_ase_cp_wr_buf),
    BTH_PATCH_FUNC(bap_uc_cli_before_cfg_uc_stream_grp),
#endif
#if (BAP_SCAN_DELEGATOR)
    BTH_PATCH_FUNC(bap_bc_scan_deleg_is_bc_rx_src_proc_busy),
    BTH_PATCH_FUNC(bap_scan_deleg_get_total_sub_grp_bis_sync_req_bf),
    BTH_PATCH_FUNC(bap_scan_deleg_before_send_bc_rx_state_ntf),
#endif
#if (ACC_MCS_ENABLE)
    BTH_PATCH_FUNC(mcs_gatt_server_callback),
#endif
#if (ACC_OTS_ENABLE)
    BTH_PATCH_FUNC(ots_gatt_server_callback),
#endif
#if (ACC_TBS_ENABLE)
    BTH_PATCH_FUNC(tbs_gatt_server_callback),
#endif
#if (ATC_CSISM_ENABLE)
    BTH_PATCH_FUNC(csis_gatt_server_callback),
#endif
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_FUNC(ascs_gatt_server_callback),
#endif
#if (BAP_BASS_ENABLE)
    BTH_PATCH_FUNC(bass_gatt_server_callback),
#endif
#if (BAP_PACS_ENABLE)
    BTH_PATCH_FUNC(pacs_gatt_server_callback),
#endif
#if (ARC_AICS_ENABLE)
    BTH_PATCH_FUNC(aics_gatt_server_callback),
#endif
#if (ARC_VOCS_ENABLE)
    BTH_PATCH_FUNC(vocs_gatt_server_callback),
#endif
#if (ARC_VCS_ENABLE)
    BTH_PATCH_FUNC(vcs_gatt_server_callback),
#endif
#if (ARC_MICS_ENABLE)
    BTH_PATCH_FUNC(mics_gatt_server_callback),
#endif
#if (CAP_CAS_ENABLE)
    BTH_PATCH_FUNC(cas_gatt_server_callback),
#endif
#if (GMAP_GMAS_ENABLE)
    BTH_PATCH_FUNC(gmas_gatt_server_callback),
#endif
#if (HAP_HAS_ENABLE)
    BTH_PATCH_FUNC(has_gatt_server_callback),
#endif
#if (TMAP_TMAS_ENABLE)
    BTH_PATCH_FUNC(tmas_gatt_server_callback),
#endif
#if (ACC_MCC_ENABLE)
    BTH_PATCH_FUNC(mcc_gatt_callback),
#endif
#if (ACC_OTC_ENABLE)
    BTH_PATCH_FUNC(otc_gatt_callback),
#endif
#if (ACC_TBC_ENABLE)
    BTH_PATCH_FUNC(tbc_gatt_callback),
#endif
#if (ARC_AICC_ENABLE)
    BTH_PATCH_FUNC(aicc_gatt_callback),
#endif
#if (ARC_MICC_ENABLE)
    BTH_PATCH_FUNC(micc_gatt_callback),
#endif
#if (ARC_VCC_ENABLE)
    BTH_PATCH_FUNC(vcc_gatt_callback),
#endif
#if (ARC_VOCC_ENABLE)
    BTH_PATCH_FUNC(vocc_gatt_callback),
#endif
    BTH_PATCH_FUNC(csisc_gatt_callback),
#if (BAP_BROADCAST_ASSIST)
    BTH_PATCH_FUNC(bap_bc_assist_gatt_callback),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_FUNC(ascc_gatt_callback),
#endif
#if (BAP_PACC_ENABLE)
    BTH_PATCH_FUNC(pacc_gatt_callback),
#endif
#if (CAP_CAC_ENABLE)
    BTH_PATCH_FUNC(cac_gatt_callback),
#endif
#if (GMAP_GMAC_ENABLE)
    BTH_PATCH_FUNC(gmac_gatt_callback),
#endif
#if (HAP_HAC_ENABLE)
    BTH_PATCH_FUNC(hac_gatt_callback),
#endif
#if (TMAP_TMAC_ENABLE)
    BTH_PATCH_FUNC(tmac_gatt_callback),
#endif
    BTH_PATCH_FUNC(gen_aud_codec_cfg_check_param),
    BTH_PATCH_FUNC(gen_aud_codec_capa_check_param),
    BTH_PATCH_FUNC(gaf_log_func_internal),
    BTH_PATCH_FUNC(gaf_log_dump_internal),
    BTH_PATCH_FUNC(gen_aud_codec_is_id_valid),
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_FUNC(bap_uc_srv_bap_stream_event_callback),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_FUNC(bap_uc_cli_stream_event_callback),
#endif
};

#endif /* BUILD_BTH_ROM */
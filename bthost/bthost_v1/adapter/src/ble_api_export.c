#include "bluetooth_ble_api.h"
#include "co_bt_defines.h"
#include "gatt_service.h"
#include "app_ble.h"
#if BLE_ISO_ENABLED
#include "isoohci_int.h"
#endif
#if BLE_AUDIO_ENABLED
#include "ble_audio_earphone_info.h"
#include "ble_audio_test.h"
#include "ble_audio_core.h"
#include "ble_audio_core_api.h"
#include "ble_audio_mobile_info.h"

#include "aob_conn_api.h"
#include "aob_csip_api.h"
#include "aob_gatt_cache.h"
#include "aob_bis_api.h"
#include "aob_call_api.h"
#include "aob_volume_api.h"
#include "aob_media_api.h"
#include "aob_pacs_api.h"
#include "aob_gaf_api.h"
#include "aob_stream_handler.h"
#include "aob_service_sync.h"

#include "ble_aob_common.h"

#include "app_bap_data_path_itf.h"

#ifdef BLE_USB_AUDIO_SUPPORT
#include "app_ble_usb_audio.h"
#endif  //BLE_USB_AUDIO_SUPPORT
#endif  //BLE_AUDIO_ENABLED

#include "gaf_media_common.h"

#ifdef BLE_WALKIE_TALKIE
#include "walkie_talkie_ble_gapm_cmd.h"
#endif

#if defined(IBRT_UI)
#include "app_tws_ibrt_conn_api.h"
#include "app_custom_api.h"
#endif

#ifdef BLE_RATE_TEST_SERVER
#include "app_rate_tests.h"
#include "bes_rate_test_api.h"
#endif

#ifdef BLE_RATE_TEST_CLIENT
#include "app_rate_testc.h"
#include "bes_rate_test_api.h"
#endif
/**
 * BLE HOST INIT API
 *
 */
void bes_ble_app_init(nvrec_appmode_e app_mode)
{
#if (BLE_AUDIO_ENABLED)
    /// trans app_mode to bes aob aud cfg
    aob_audio_cfg_e bes_aob_aud_cfg = AOB_AUD_CFG_MIN;

    switch (app_mode)
    {
        case NV_APP_EARBUDS_MONO:
            bes_aob_aud_cfg = AOB_AUD_CFG_TWS_MONO;
            break;
        case NV_APP_EARBUDS_STEREO_ONE_CIS:
            bes_aob_aud_cfg = AOB_AUD_CFG_TWS_STEREO_ONE_CIS;
            break;
        case NV_APP_EARBUDS_STEREO_TWO_CIS:
            bes_aob_aud_cfg = AOB_AUD_CFG_TWS_STEREO_TWO_CIS;
            break;
        case NV_APP_HEADSET_STEREO_ONE_CIS:
            bes_aob_aud_cfg = AOB_AUD_CFG_FREEMAN_STEREO_ONE_CIS;
            break;
        case NV_APP_HEADSET_STEREO_TWO_CIS:
            bes_aob_aud_cfg = AOB_AUD_CFG_FREEMAN_STEREO_TWO_CIS;
            break;
        default:
            break;
    }

#if defined(IBRT_UI)
    app_custom_ui_bt_ctx_init();
#endif

    ble_audio_core_init();

#if (IBRT)
    ble_audio_tws_sync_init();
#endif

    ble_audio_earphone_info_init();

    ble_audio_earphone_info_set_audido_cfg_select(bes_aob_aud_cfg);

    bes_ble_audio_common_init();

#ifdef AOB_MOBILE_ENABLED
    ble_audio_mobile_info_init();
#ifdef BLE_USB_AUDIO_SUPPORT
    app_ble_usb_audio_init();
#endif  //BLE_USB_AUDIO_SUPPORT
#endif  //AOB_MOBILE_ENABLED

#else  //BLE_AUDIO_ENABLED
    app_ble_stub_user_init();
#endif //BLE_AUDIO_ENABLED

#ifdef CUSTOMER_DEFINE_ADV_DATA
    app_ble_custom_init();
#endif
}

void bes_ble_app_reinit()
{

}

/**
 * BLE STACK API
 *
 */
void bes_ble_stack_init(void)
{

}

void bes_ble_init_ble_name(const char *name)
{

}

void bes_ble_ke_event_schedule(void)
{

}

void bes_ble_reset_start(void)
{

}

/**
 * IBRT CALL FUNC
 *
 */
void bes_ble_roleswitch_start(void)
{
    ble_roleswitch_start();
}

void bes_ble_roleswitch_complete(uint8_t newRole)
{
    ble_roleswitch_complete(newRole);
}

void bes_ble_role_update(uint8_t newRole)
{
    ble_role_update(newRole);
}

void bes_ble_ibrt_event_entry(uint8_t ibrt_evt_type)
{
    ble_ibrt_event_entry(ibrt_evt_type);
}

/**
 * GAP
 *
 */
void bes_ble_gap_stub_user_init(void)
{
    app_ble_stub_user_init();
}

void bes_ble_gap_start_connect(bes_ble_bdaddr_t *addr, BES_GAP_OWN_ADDR_E own_type)
{
    app_ble_start_connect((ble_bdaddr_t *)addr, own_type);
}

void bes_ble_gap_connect_ble_audio_device(bes_ble_bdaddr_t *addr, BES_GAP_OWN_ADDR_E own_type)
{
    app_ble_connect_ble_audio_device((ble_bdaddr_t *)addr, own_type);
}

void bes_ble_gap_cancel_connecting(void)
{
    app_ble_cancel_connecting();
}

bool bes_ble_gap_is_connection_on(uint8_t index)
{
    return app_ble_is_connection_on(index);
}

void bes_ble_gap_update_conn_param_mode(BLE_CONN_PARAM_MODE_E mode, bool isEnable)
{
    app_ble_update_conn_param_mode(mode, isEnable);
}

bool bes_ble_gap_is_remote_mobile_connected(const ble_bdaddr_t* p_addr)
{
    return app_ble_is_remote_mobile_connected(p_addr);
}

int8_t bes_ble_gap_get_rssi(uint8_t conidx)
{
    return app_ble_get_rssi(conidx);
}

void bes_ble_gap_clear_white_list_for_mobile(void)
{
    app_ble_clear_white_list(BLE_WHITE_LIST_USER_MOBILE);
}

void bes_ble_gap_start_disconnect(uint8_t conIdx)
{
    app_ble_start_disconnect(conIdx);
}

void bes_ble_gap_disconnect_all(void)
{
    app_ble_disconnect_all();
}

void bes_ble_gap_disconnect_by_addr(const ble_bdaddr_t *peer_addr)
{
    uint8_t conidx = 0;
    ble_bdaddr_t addr_get = {0};
    bool ret = false;

    for (conidx = 0; conidx < AOB_COMMON_MOBILE_CONNECTION_MAX; conidx++)
    {
        ret = bes_ble_gap_get_peer_solved_addr(conidx, &addr_get);

        if (ret == true && !memcmp(peer_addr, &addr_get, sizeof(addr_get)))
        {
            bes_ble_gap_start_disconnect(conidx);
            return;
        }
    }
}

void bes_ble_gap_refresh_irk(void)
{

}

void bes_ble_gap_force_switch_adv(enum BLE_ADV_SWITCH_USER_E user, bool isToEnableAdv)
{
    app_ble_force_switch_adv(user, isToEnableAdv);
}

void bes_ble_gap_start_connectable_adv(uint16_t advInterval)
{
    app_ble_start_connectable_adv(advInterval);
}

void bes_ble_gap_set_white_list(BLE_WHITE_LIST_USER_E user, const ble_bdaddr_t *bdaddr, uint8_t size)
{
    app_ble_set_white_list(user, (ble_bdaddr_t *)bdaddr, size);
}

void bes_ble_gap_remove_white_list_user_item(BLE_WHITE_LIST_USER_E user)
{
    app_ble_clear_white_list(user);
}

void bes_ble_gap_clear_white_list(void)
{
    app_ble_clear_all_white_list();
}

void bes_ble_gap_set_rpa_list(const ble_bdaddr_t *ble_addr, const uint8_t *irk)
{
    app_ble_add_dev_to_rpa_list_in_controller(ble_addr, irk);
}

void bes_ble_gap_set_bonded_devs_rpa_list(void)
{
    app_ble_add_devices_info_to_resolving();
}

void bes_ble_gap_set_rpa_timeout(uint16_t rpa_timeout)
{
    gap_set_rpa_timeout(rpa_timeout);
}

void bes_ble_gap_start_three_adv(uint32_t BufPtr, uint32_t BufLen)
{
    app_ble_start_three_adv_test();
}

void bes_ble_gap_stop_all_adv(uint32_t BufPtr, uint32_t BufLen)
{
    gap_disable_all_advertising(GAP_ADV_DISABLE_ALL_BY_UPPER_APP);
}

void bes_ble_gap_custom_adv_start(BLE_ADV_ACTIVITY_USER_E actv_user)
{
    app_ble_custom_adv_start(actv_user);
}

BLE_ADV_ACTIVITY_USER_E bes_ble_param_get_actv_user_from_adv_user(BLE_ADV_USER_E user)
{
    return app_ble_param_get_actv_user_from_adv_user(user);
}

void bes_ble_gap_custom_adv_write_data(bes_ble_gap_cus_adv_param_t *param)
{
    app_ble_custom_adv_write_data(param->actv_user, param->is_custom_adv_flags, param->type,
                                  param->local_addr, (ble_bdaddr_t *)param->peer_addr,param->adv_interval,
                                  param->adv_type, param->adv_mode, param->tx_power_dbm,
                                  param->adv_data, param->adv_data_size, param->scan_rsp_data,
                                  param->scan_rsp_data_size);
}

void bes_ble_gap_custom_adv_stop(BLE_ADV_ACTIVITY_USER_E actv_user)
{
    app_ble_custom_adv_stop(actv_user);
}

void bes_ble_gap_custom_refresh_adv_state()
{
    app_ble_refresh_adv_state_by_custom_adv(0);
}

void bes_ble_gap_refresh_adv_state(uint16_t advInterval)
{
    app_ble_refresh_adv_state(advInterval);
}

void bes_ble_gap_param_set_adv_interval(BLE_ADV_INTERVALREQ_USER_E adv_intv_user, BLE_ADV_USER_E adv_user, uint32_t interval)
{
    app_ble_param_set_adv_interval(adv_intv_user, adv_user, interval);
}

void bes_ble_gap_set_all_adv_txpwr(int8_t txpwr_dbm)
{

}

bool bes_ble_is_connection_on_by_index(uint8_t conidx)
{
    return false;
}

bool bes_ble_gap_is_in_advertising_state(void)
{
    return app_ble_is_in_advertising_state();
}

void bes_ble_gap_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy, uint16_t scanWindow, uint16_t scanInterval)
{
    app_ble_start_scan(scanFilterPolicy, scanWindow, scanInterval);
}

void bes_ble_gap_stop_scan(void)
{
    app_ble_stop_scan();
}

void bes_ble_gap_ready_and_init_done(nvrec_appmode_e mode)
{
    app_ble_ready_and_init_done(mode);
}

void bes_ble_gap_adv_report_callback_register(bes_ble_adv_data_report_cb_t cb)
{
    app_ble_customif_adv_report_callback_register((ble_adv_data_report_cb_t)cb);
}

void bes_ble_gap_adv_report_callback_deregister(void)
{
    app_ble_customif_adv_report_callback_deregister();
}

void bes_ble_customif_link_event_callback_register(bes_ble_link_event_report_cb_t cb)
{
    app_ble_customif_link_event_callback_register((ble_link_event_report_cb_t)cb);
}

void bes_ble_customif_link_event_callback_deregister(void)
{
    app_ble_customif_link_event_callback_deregister();
}

void bes_ble_set_tx_rx_pref_phy(uint32_t tx_pref_phy, uint32_t rx_pref_phy)
{
    app_ble_set_tx_rx_pref_phy(tx_pref_phy, rx_pref_phy);
}

void bes_ble_connect_req_callback_register(bes_ble_link_connect_cb_t req_cb, bes_ble_link_connect_cb_t done_cb)
{

}

void bes_ble_connect_req_callback_deregister(void)
{

}

void bes_ble_mtu_exec_ind_callback_register(bes_ble_link_mtu_exch_cb_t mtu_exec_cb)
{
    app_ble_mtu_exec_ind_callback_register(mtu_exec_cb);
}

void bes_ble_mtu_exec_ind_callback_deregister(void)
{
    app_ble_mtu_exec_ind_callback_deregister();
}

void bes_ble_gatt_cli_create_bearer(uint8_t conidx)
{
    gatt_create_att_bearer(gap_conn_idx_as_hdl(conidx));
}

void bes_ble_gatt_cli_disconnect_bearer(uint8_t conidx)
{
    gatt_disconnect_att_bearers(gap_conn_idx_as_hdl(conidx), false, 0);
}

void bes_ble_set_scan_coded_phy_en_and_param_before_start_scan(bool enable, bes_ble_scan_wd_t *start_scan_coded_scan_wd)
{

}

void bes_ble_set_init_conn_all_phy_param_before_start_connect(bes_ble_conn_param_t *init_param_universal,
                                                        bes_ble_scan_wd_t *init_coded_scan_wd)
{

}

void bes_ble_gap_register_data_fill_handle(BLE_ADV_USER_E user, BLE_DATA_FILL_FUNC_T func, bool enable)
{
    app_ble_register_data_fill_handle(user, func, enable);
}

void bes_ble_gap_data_fill_enable(BLE_ADV_USER_E user, bool enable)
{
    app_ble_data_fill_enable(user, enable);
}

bool bes_ble_gap_is_any_connection_exist(void)
{
    return app_ble_is_any_connection_exist();
}

uint8_t bes_ble_gap_connection_count(void)
{
    return app_ble_connection_count();
}

uint16_t bes_ble_gap_get_conhdl_from_conidx(uint8_t conidx)
{
    return app_ble_get_conhdl_from_conidx(conidx);
}

void bes_ble_gap_conn_update_param(uint8_t conidx, uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
        uint32_t supervision_timeout_in_ms, uint8_t  slaveLatency)
{
    gap_update_params_t param = {0};
    param.conn_interval_min_1_25ms = (min_interval_in_ms * 100) / 125; // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4000ms
    param.conn_interval_max_1_25ms = (max_interval_in_ms * 100) / 125; // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4000ms
    param.max_peripheral_latency = slaveLatency;
    param.superv_timeout_ms = supervision_timeout_in_ms;
    gap_update_le_conn_parameters(gap_zero_based_ble_conidx_as_hdl(conidx), &param);
}

void bes_ble_gap_l2cap_data_rec_over_bt(uint8_t condix, uint16_t conhdl, uint8_t* ptrData, uint16_t dataLen)
{

}

void bes_ble_gap_core_register_global_handler(APP_BLE_CORE_GLOBAL_HANDLER_FUNC handler)
{
    app_ble_core_register_global_handler_ind(handler);
}

ble_bdaddr_t bes_ble_gap_get_current_ble_addr(void)
{
    ble_bdaddr_t ble_addr = {{0},0};
    ble_addr = app_get_current_ble_addr();
    return ble_addr;
}

ble_bdaddr_t bes_ble_gap_get_local_identity_addr(uint8_t conidx)
{
    ble_bdaddr_t ble_addr = {{0},0};
    ble_addr = app_ble_get_local_identity_addr(conidx);
    return ble_addr;
}

const uint8_t *bes_ble_gap_get_local_rpa_addr(uint8_t conidx)
{
    return app_ble_get_local_rpa_addr(conidx);
}

void bes_ble_gap_sec_send_security_req(uint8_t conidx, uint8_t sec_level)
{
    app_ble_send_security_req(conidx);
}

void bes_ble_gap_sec_reg_dist_lk_bit_set_callback(set_rsp_dist_lk_bit_field_func callback)
{
    app_sec_reg_dist_lk_bit_set_callback(callback);
}

set_rsp_dist_lk_bit_field_func bes_ble_gap_sec_reg_dist_lk_bit_get_callback()
{
    return app_sec_reg_dist_lk_bit_get_callback();
}

void bes_ble_gap_sec_reg_smp_identify_info_cmp_callback(smp_identify_addr_exch_complete callback)
{
    app_ble_register_ia_exchanged_callback(callback);
}

bool bes_ble_gap_get_peer_solved_addr(uint8_t conidx, ble_bdaddr_t* p_addr)
{
    return app_ble_get_peer_solved_addr(conidx, p_addr);
}

void bes_ble_gap_get_tx_pwr_value(uint8_t conidx, bes_ble_tx_object_e obj, bes_ble_phy_pwr_value_e phy)
{
    if (obj == BES_BLE_TX_LOCAL)
    {
        gap_read_conn_local_tx_power(gap_zero_based_ble_conidx_as_hdl(conidx), (gap_le_detail_phy_t)(phy+1));
    }
    else
    {
        gap_read_conn_remote_tx_power(gap_zero_based_ble_conidx_as_hdl(conidx), (gap_le_detail_phy_t)(phy+1));
    }
}

void bes_ble_gap_get_dev_tx_pwr_range(void)
{
    gap_read_le_tx_power_range();
}

void bes_ble_gap_get_adv_txpower_value(void)
{
    gap_read_legacy_adv_tx_power();
}

void bes_ble_gap_tx_power_report_enable(uint8_t conidx, bool local_enable, bool remote_enable)
{
    gap_set_conn_tx_power_report(gap_zero_based_ble_conidx_as_hdl(conidx), local_enable, remote_enable);
}

void bes_ble_gap_subrate_request(uint8_t conidx, uint16_t subrate_min, uint16_t subrate_max,
        uint16_t latency_max, uint16_t cont_num, uint16_t timeout)
{
    gap_subrate_params_t param = {0};
    param.subrate_factor_min = subrate_min;
    param.subrate_factor_max = subrate_max;
    param.max_peripheral_latency = latency_max;
    param.conn_continuation_number = cont_num;
    param.superv_timeout_ms = timeout;
    if (conidx == GAP_INVALID_CONIDX)
    {
        gap_set_default_subrate(&param);
    }
    else
    {
        gap_update_subrate_parameters(gap_zero_based_ble_conidx_as_hdl(conidx), &param);
    }
}

void bes_ble_gap_set_phy_mode(uint8_t conidx, bes_le_phy_val_e phy_val, bes_le_phy_opt_e phy_opt)
{
    uint8_t tx_phy_bits = phy_val ? (1 << (phy_val - 1)) : 0;
    uint8_t rx_phy_bits = phy_val ? (1 << (phy_val - 1)) : 0;
    gap_coded_phy_prefer_t coded_prefer = GAP_CODED_PHY_NO_PREFER_CODING;
    if (phy_val == BES_BLE_PHY_CODED_VALUE)
    {
        coded_prefer = (gap_coded_phy_prefer_t)phy_opt;
    }
    gap_set_le_conn_phy(gap_zero_based_ble_conidx_as_hdl(conidx), tx_phy_bits, rx_phy_bits, coded_prefer);
}

void bes_ble_gap_get_phy_mode(uint8_t conidx)
{
    gap_read_le_conn_phy(gap_zero_based_ble_conidx_as_hdl(conidx));
}

/**
 * WALKIE_TALKIE
 *
 */
#ifdef BLE_WALKIE_TALKIE
void bes_ble_walkie_stop_activity(uint8_t actv_idx){
     walkie_stop_activity(actv_idx);
}

void bes_ble_walkie_delete_activity(uint8_t actv_idx){
     walkie_delete_activity(actv_idx);
}

void bes_ble_walkie_adv_creat(uint8_t *adv_para){
    walkie_adv_creat(adv_para);
}

void bes_ble_walkie_adv_enable(uint16_t duration, uint8_t max_adv_evt, uint8_t actv_idx){
    walkie_adv_enable(duration, max_adv_evt, actv_idx);
}

void bes_ble_walkie_set_adv_data_cmd(uint8_t operation, uint8_t actv_idx,
                                      uint8_t *adv_data, uint8_t data_len){
     walkie_set_adv_data_cmd(operation, actv_idx, adv_data, data_len);
}

void bes_ble_walkie_periodic_sync_create(uint8_t own_addr_type){
     walkie_periodic_sync_create(own_addr_type);
}

void bes_ble_walkie_periodic_sync_enable(uint8_t actv_idx, uint8_t *per_para){
     walkie_periodic_sync_enable(actv_idx, per_para);
}

void bes_ble_walkie_scan_creat(uint8_t own_addr_type){
    walkie_scan_creat(own_addr_type);
}

void bes_ble_walkie_scan_enable(uint8_t actv_idx,uint8_t *scan_para){
    walkie_scan_enable(actv_idx, scan_para);
}

void bes_ble_walkie_set_device_list(uint8_t list_type, uint8_t *bdaddr, uint8_t size){
    walkie_set_device_list(list_type, bdaddr, size);
}

void bes_ble_walkie_ble_gapm_task_init(bes_if_walie_gap_cb_func_t * cb){
     walkie_ble_gapm_task_init((walie_gap_cb_func_t *)cb);
}

#ifdef BLE_ISO_ENABLED_X
bool bes_ble_walkie_send_big_create_sync_hci_cmd(const bes_if_walkie_gap_big_info_t* p_big_info){
    return walkie_send_big_create_sync_hci_cmd((const walkie_gap_big_info_t*)p_big_info);
}

void bes_ble_walkie_talkie_print_big_info(const bes_if_walkie_gap_big_info_t* p_big_info){
    walkie_talkie_print_big_info((const walkie_gap_big_info_t*)p_big_info);
}

void bes_ble_walkie_talkie_big_src_create(){
    walkie_talkie_big_src_create();
}

bool bes_ble_walkie_bc_src_start_streaming(){
    return walkie_bc_src_start_streaming();
}

void bes_ble_walkie_bc_src_stop_streaming(){
    walkie_bc_src_stop_streaming();
}

void bes_ble_walkie_bc_src_send_data(uint8_t *payload, uint16_t payload_len){
    walkie_bc_src_send_data(payload, payload_len);
}

uint8_t bes_ble_walkie_bis_rx_stream_start_by_con_hdl(uint16_t con_hdl){
    return walkie_bis_rx_stream_start_by_con_hdl(con_hdl);
}

uint8_t bes_ble_walkie_bis_rx_stream_start(uint8_t actv_idx){
    return walkie_bis_rx_stream_start(actv_idx);
}

void bes_ble_walkie_bis_rx_stream_stop(uint8_t actv_idx){
    walkie_bis_rx_stream_stop(actv_idx);
}

void bes_ble_walkie_bis_register_recv_iso_data_callback(){
    walkie_bis_register_recv_iso_data_callback();
}

void bes_ble_walkie_bis_unregister_recv_iso_data_callback(){
    walkie_bis_unregister_recv_iso_data_callback();
}
#endif  //BLE_ISO_ENABLED
#endif  //BLE_WALKIE_TALKIE

void bes_ble_iso_quality(uint16_t cisHdl, uint8_t *param)
{
#if BLE_AUDIO_ENABLED
    app_bap_uc_srv_iso_quality_ind_handler(cisHdl, param);
#endif
}

/**
 * Datapath
 *
 */

#if defined(BES_OTA) && !defined(OTA_OVER_TOTA_ENABLED)
void bes_ble_ota_event_reg(bes_ble_ota_event_callback cb)
{
    app_ota_event_reg((app_ota_event_callback)cb);
}

void bes_ble_ota_event_unreg(void)
{
    app_ota_event_unreg();
}

void bes_ble_ota_send_rx_cfm(uint8_t conidx)
{
    app_ota_send_rx_cfm(conidx);
}

bool bes_ble_ota_send_notification(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    return app_ota_send_notification(conidx, ptrData, length);
}

bool bes_ble_ota_send_indication(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    return app_ota_send_indication(conidx, ptrData, length);
}
#endif

#ifdef BLE_TOTA_ENABLED
void bes_ble_tota_event_reg(bes_ble_tota_event_callback cb)
{
    app_tota_event_reg((app_tota_event_callback)cb);
}

void bes_ble_tota_event_unreg(void)
{
    app_tota_event_unreg();
}

bool bes_ble_tota_send_notification(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    return app_tota_send_notification(conidx, ptrData, length);
}

bool bes_ble_tota_send_indication(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    return app_tota_send_indication(conidx, ptrData, length);
}
#endif

#ifdef __AI_VOICE_BLE_ENABLE__
void bes_ble_ai_gatt_event_reg(bes_ble_ai_event_cb cb)
{
    app_ai_event_reg((app_ble_ai_event_cb)cb);
}

void bes_ble_ai_gatt_data_send(bes_ble_ai_data_send_param_t *param)
{
    app_ai_data_send((app_ble_ai_data_send_param_t *) param);
}
#endif

#ifdef TILE_DATAPATH
void bes_ble_tile_event_cb_reg(bes_ble_tile_event_cb cb)
{
    app_tile_event_cb_reg((app_ble_tile_event_cb)cb);
}

void bes_ble_tile_send_via_notification(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    app_tile_send_ntf(conidx, ptrData, length);
}
#endif

/**
 * DATAPATH_SERVER
 *
 */
#ifdef CFG_APP_DATAPATH_SERVER
void bes_ble_datapath_server_control_notification(uint8_t conidx, bool isEnable)
{
    app_datapath_server_control_notification(conidx, isEnable);
}

void bes_ble_datapath_server_send_data_via_notification(uint8_t* ptrData, uint32_t length)
{
    app_datapath_server_send_data_via_notification(ptrData, length);
}

void bes_ble_datapath_server_send_data_via_intification(uint8_t* ptrData, uint32_t length)
{
    app_datapath_server_send_data_via_indication(ptrData, length);
}

void bes_ble_datapath_server_send_data_via_write_command(uint8_t* ptrData, uint32_t length)
{
    app_datapath_server_send_data_via_write_command(ptrData, length);
}

void bes_ble_datapath_server_send_data_via_write_request(uint8_t* ptrData, uint32_t length)
{
    app_datapath_server_send_data_via_write_request(ptrData, length);
}

void bes_ble_add_datapathps(void)
{

}

void bes_ble_datapath_server_register_event_cb(app_datapath_event_cb callback)
{
    app_datapath_server_register_event_cb(callback);
}
#endif

/**
 * GFPS
 *
 */

#ifdef GFPS_ENABLED
void bes_ble_gfps_init(app_ble_adv_activity_func func)
{
    ble_app_gfps_init(func);
}
#ifdef SPOT_ENABLED
void bes_ble_spot_init(app_ble_adv_activity_func func)
{
    ble_app_spot_init(func);
}
#endif

uint8_t bes_ble_gfps_l2cap_send(uint8_t conidx, uint8_t *ptrData, uint32_t length)
{
    return ble_app_gfps_l2cap_send(conidx, ptrData, length);
}

void bes_ble_gfps_l2cap_disconnect(uint8_t conidx)
{
    ble_app_gfps_l2cap_disconnect(conidx);
}

void bes_ble_gfps_send_keybase_pairing(uint8_t conidx, uint8_t *data, uint16_t length)
{
    ble_app_gfps_send_keybase_pairing(conidx, data, length);
}

#ifdef SPOT_ENABLED
void bes_ble_gfps_send_beacon_data(uint8_t conidx, uint8_t *data, uint16_t length)
{
    ble_app_gfps_send_beacon_data(conidx, data, length);
}
#endif
#endif /* BLE_APP_GFPS */

/**
 * BLE AUDIO
 *
 */

#if BLE_AUDIO_ENABLED
void bes_ble_audio_common_init(void)
{
#ifdef BLE_AUDIO_TEST_ENABLED
    ble_audio_dflt_update_role();
    ble_audio_dflt_config();
#else  //BLE_AUDIO_TEST_ENABLED
    ble_audio_set_tws_nv_role_via_nv_addr();
    ble_audio_tws_init();   //Normal init cfg
#endif  // BLE_AUDIO_TEST_ENABLED
}

void bes_ble_audio_common_deinit(void)
{
    ble_audio_tws_deinit();
}

bool bes_ble_audio_make_new_le_core_sm(uint8_t conidx, uint8_t *peer_bdaddr)
{
    return ble_audio_make_new_le_core_sm(conidx, peer_bdaddr);
}

void bes_ble_audio_core_register_event_cb(const BLE_AUD_CORE_EVT_CB_T *cb)
{
    ble_audio_core_register_event_cb(cb);
}

bool bes_ble_audio_is_ux_mobile(void)
{
    return ble_audio_is_ux_mobile();
}

uint8_t bes_ble_audio_get_mobile_connected_lid(uint8_t con_lid[])
{
    return ble_audio_get_mobile_connected_dev_lids(con_lid);
}

void bes_ble_audio_test_update_role(void)
{
    ble_audio_dflt_update_role();
}

BLE_AUDIO_POLICY_CONFIG_T* bes_ble_audio_get_policy_config(void)
{
    return app_ble_audio_get_policy_config();
}

void bes_ble_audio_mobile_req_disconnect(ble_bdaddr_t *addr)
{
    ble_audio_mobile_req_disconnect(addr);
}

void bes_ble_audio_disconnect_all_connection(void)
{
    ble_audio_disconnect_all_connection();
}

bool bes_ble_audio_is_any_mobile_connnected(void)
{
    return ble_audio_is_any_mobile_connnected();
}

#ifdef TWS_SYSTEM_ENABLED
void bes_ble_sync_ble_info(void)
{
    app_ble_sync_ble_info();
}

void bes_ble_gap_mode_tws_sync_init(void)
{
    app_ble_mode_tws_sync_init();
}
#endif

#ifdef AOB_MOBILE_ENABLED

void bes_ble_mobile_start_connect(void)
{
    ble_mobile_start_connect();
}

void bes_ble_mobile_connect_failed(bool is_failed)
{
    ble_mobile_connect_failed(is_failed);
}

bool bes_ble_mobile_is_connect_failed(void)
{
    return ble_mobile_is_connect_failed();
}

void bes_ble_audio_mobile_conn_next_paired_dev(ble_bdaddr_t* bdaddr)
{
    ble_audio_mobile_conn_next_paired_dev(bdaddr);
}

uint8_t* bes_ble_audio_mobile_conn_get_connecting_dev(void)
{
    return ble_audio_mobile_conn_get_connecting_dev();
}

void bes_lea_mobile_stream_start(uint8_t con_lid, bes_lea_ase_cfg_param_t *cfg)
{
    aob_media_mobile_start_stream((AOB_MEDIA_ASE_CFG_INFO_T *)cfg, 0, true);
}

void bes_ble_audio_mobile_core_register_event_cb(const BLE_AUD_MOB_CORE_EVT_CB_T *cb)
{
    ble_audio_mobile_core_register_event_cb(cb);
}
#endif

void bes_ble_bap_set_activity_type(gaf_bap_activity_type_e type)
{
    app_bap_set_activity_type(type);
}

void bes_ble_update_tws_nv_role(uint8_t role)
{
    ble_audio_update_tws_nv_role(role);
}

bool bes_ble_aob_conn_start_adv(bool br_edr_support, bool discoverable, bool init_reconnect)
{
    return aob_conn_start_adv(br_edr_support, discoverable, init_reconnect);
}

bool bes_ble_aob_conn_stop_adv(void)
{
    return aob_conn_stop_adv();
}

uint8_t bes_ble_audio_get_mobile_lid_by_pub_address(uint8_t *pub_addr)
{
    return ble_audio_get_mobile_lid_by_pub_address(pub_addr);
}

bool bes_ble_audio_is_mobile_link_connected(ble_bdaddr_t *addr)
{
    return ble_audio_is_mobile_link_connected(addr);
}

void bes_ble_audio_mobile_disconnect_device(uint8_t conidx)
{
    ble_audio_mobile_disconnect_device(conidx);
}

void bes_ble_aob_gattc_rebuild_cache(GATTC_NV_SRV_ATTR_t *record)
{
    aob_gattc_rebuild_cache(record);
}

void bes_ble_aob_service_recv_handler(uint8_t *p_buff, uint16_t len)
{
    app_ble_audio_recv_service_data(p_buff, len);
}

bool bes_ble_aob_csip_if_get_sirk(uint8_t *sirk)
{
    return aob_csip_if_get_sirk(sirk);
}

void bes_ble_aob_csip_if_use_temporary_sirk()
{
    aob_csip_if_use_temporary_sirk();
}

void bes_ble_aob_csip_if_refresh_sirk(uint8_t *sirk)
{
    aob_csip_if_refresh_sirk(sirk);
}

bool bes_ble_aob_csip_sirk_already_refreshed(void)
{
    return aob_csip_sirk_already_refreshed();
}

void bes_ble_aob_csip_if_update_sirk(uint8_t *sirk, uint8_t sirk_len)
{
    aob_csip_if_update_sirk(sirk, sirk_len);
}

bool bes_ble_aob_csip_is_use_custom_sirk(void)
{
    return aob_csip_is_use_custom_sirk();
}

void bes_ble_aob_conn_dump_state_info(void)
{
    aob_conn_dump_state_info();
}

void bes_ble_aob_bis_tws_sync_state_req(void)
{
#ifdef APP_BLE_BIS_SINK_ENABLE
    aob_bis_tws_sync_state_req();
#endif
}

void bes_ble_aob_bis_tws_sync_state_req_handler(uint8_t *buf)
{
#ifdef APP_BLE_BIS_SINK_ENABLE
    aob_bis_tws_sync_state_req_handler(buf);
#endif
}

ble_bdaddr_t *bes_ble_aob_conn_get_remote_address(uint8_t con_lid)
{
    return aob_conn_get_remote_address(con_lid);
}

void bes_ble_aob_media_play(uint8_t con_lid)
{
    aob_media_play(con_lid);
}

void bes_ble_aob_media_pause(uint8_t con_lid)
{
    aob_media_pause(con_lid);
}

void bes_ble_aob_media_stop(uint8_t con_lid)
{
    aob_media_stop(con_lid);
}

void bes_ble_aob_media_next(uint8_t con_lid)
{
    aob_media_next(con_lid);
}

void bes_ble_aob_media_prev(uint8_t con_lid)
{
    aob_media_prev(con_lid);
}

void bes_ble_aob_vol_mute(void)
{
    aob_vol_mute();
}

void bes_ble_aob_vol_unmute(void)
{
    aob_vol_unmute();
}

void bes_ble_aob_vol_up(void)
{
    aob_vol_up();
}

void bes_ble_aob_vol_down(void)
{
    aob_vol_down();
}

void bes_ble_aob_call_if_outgoing_dial(uint8_t conidx, uint8_t *uri, uint8_t uriLen)
{
    aob_call_if_outgoing_dial(conidx, uri, uriLen);
}

void bes_ble_aob_call_if_retrieve_call(uint8_t conidx, uint8_t call_id)
{
    aob_call_if_retrieve_call(conidx, call_id);
}

void bes_ble_aob_call_if_hold_call(uint8_t conidx, uint8_t call_id)
{
    aob_call_if_hold_call(conidx, call_id);
}

void bes_ble_aob_call_if_terminate_call(uint8_t conidx, uint8_t call_id)
{
    aob_call_if_terminate_call(conidx, call_id);
}

void bes_ble_aob_call_if_accept_call(uint8_t conidx, uint8_t call_id)
{
    aob_call_if_accept_call(conidx, call_id);
}

uint8_t bes_ble_aob_convert_local_vol_to_le_vol(uint8_t bt_vol)
{
    return aob_convert_local_vol_to_le_vol(bt_vol);
}

uint8_t bes_ble_audio_get_mobile_sm_index_by_addr(ble_bdaddr_t *addr)
{
    return ble_audio_get_mobile_sm_index_by_addr(addr);
}

void bes_ble_audio_sink_streaming_handle_event(uint8_t con_lid, uint8_t data,
                                                                bes_gaf_direction_t direction, app_ble_audio_event_t event)
{
    app_ble_audio_sink_streaming_handle_event(con_lid, data, (app_gaf_direction_t)direction, event);
}

uint8_t bes_ble_audio_get_mobile_addr(uint8_t deviceId, uint8_t *addr)
{
    AOB_MOBILE_INFO_T *pLEMobileInfo = ble_audio_earphone_info_get_mobile_info(deviceId);
    if(NULL != pLEMobileInfo)
    {

        memcpy(addr, pLEMobileInfo->peer_ble_addr.addr, 6);
    }
    else
    {
        return -1;
    }
    return 0;
}

void bes_ble_audio_dump_conn_state(void)
{
    ble_adv_activity_t *p_actv_le_audio = app_ble_get_advertising_by_user(USER_BLE_AUDIO);

    if (p_actv_le_audio != NULL && p_actv_le_audio->adv_is_started)
    {
        TRACE(3, "BLE adv state: 5, adv type: %d, interval: %d, busy state: 0",
              !p_actv_le_audio->adv_param.use_legacy_pdu && p_actv_le_audio->adv_param.connectable ? 5 : 0,
              p_actv_le_audio->custom_adv_interval_ms);
    }
    else
    {
        TRACE(1, "BLE adv state: 0, adv type: 0, interval: 0, busy state: 0");
    }
    // Judge ble whether have connect handle
    if (app_ble_is_any_connection_exist())
    {
        TRACE(1, "BLE cnn state: %d", BLE_CONNECTED);
    }
    else
    {
        TRACE(1, "BLE cnn state: %d", BLE_DISCONNECTED);
    }

    for (uint8_t index = 0; index < AOB_COMMON_MOBILE_CONNECTION_MAX; index++)
    {
        // dump mobile state
        AOB_MOBILE_INFO_T *p_mobileInfo = ble_audio_earphone_info_get_mobile_info(index);

        if (NULL != p_mobileInfo && true == p_mobileInfo->connected)
        {
            BLE_ADDR_INFO_T bleAddr = p_mobileInfo->peer_ble_addr;
            TRACE(9, "[BLE Audio]: mobile: %d mute: %d, volume: %d, address: %02x:%02x:%02x:%02x:%02x:%02x",
                p_mobileInfo->conidx, p_mobileInfo->muted, p_mobileInfo->volume,
                bleAddr.addr[0], bleAddr.addr[1], bleAddr.addr[2],bleAddr.addr[3], bleAddr.addr[4], bleAddr.addr[5]);
            //dump ble cis and ase state

            app_bap_ascs_ase_t *p_ase = app_bap_uc_srv_get_ase_info(p_mobileInfo->conidx);

            if (NULL != p_ase)
            {
                TRACE(3, "[BLE Audio]: index: %d, cis state: %d, handle: %d", p_mobileInfo->conidx, p_ase->con_lid, p_ase->cis_hdl);
            }
        }
    }
}

uint8_t bes_ble_aob_get_call_id_by_conidx_and_type(uint8_t device_id, uint8_t call_state)
{
    return ble_audio_earphoe_info_get_call_id_by_conidx_and_type(device_id, call_state);
}

uint8_t bes_ble_aob_get_call_id_by_conidx(uint8_t device_id)
{
    return ble_audio_earphone_info_get_call_id_by_conidx(device_id);
}

bool bes_ble_aob_get_acc_bond_status(uint8_t conidx, uint8_t type)
{
    return ble_audio_earphone_info_get_acc_bond_status(conidx, type);
}

//// BLE bis api
// bis src
uint16_t bes_ble_bis_src_get_bis_hdl_by_big_idx(uint8_t big_idx)
{
    return aob_bis_src_get_bis_hdl_by_big_idx(big_idx);
}

const AOB_CODEC_ID_T *bes_ble_bis_src_get_codec_id_by_big_idx(uint8_t big_idx, uint8_t subgrp_idx)
{
    return aob_bis_src_get_codec_id_by_big_idx(big_idx, subgrp_idx);
}

const AOB_BAP_CFG_T *bes_ble_bis_src_get_codec_cfg_by_big_idx(uint8_t big_idx)
{
    return aob_bis_src_get_codec_cfg_by_big_idx(big_idx);
}

#ifdef AOB_MOBILE_ENABLED
uint8_t bes_ble_bis_src_send_iso_data_to_all_channel(uint8_t **payload, uint16_t payload_len, uint32_t ref_time)
{
    return app_bap_bc_src_iso_send_data_to_all_channel(payload, payload_len, ref_time);
}
#endif /// AOB_MOBILE_ENABLED

void bes_ble_bis_src_set_big_param(uint8_t big_idx, bes_ble_bis_src_big_param_t *p_big_param)
{
    aob_bis_src_set_big_param(big_idx, (aob_bis_src_big_param_t *)p_big_param);
}

void bes_ble_bis_src_set_subgrp_param(uint8_t big_idx, bes_ble_bis_src_subgrp_param_t *p_subgrp_param)
{
    aob_bis_src_set_subgrp_param(big_idx, (aob_bis_src_subgrp_param_t *)p_subgrp_param);
}

void bes_ble_bis_src_set_stream_param(uint8_t big_idx, bes_ble_bis_src_stream_param_t *p_stream_param)
{
    aob_bis_src_set_stream_param(big_idx, (aob_bis_src_stream_param_t *)p_stream_param);
}

void bes_ble_bis_src_update_metadata(uint8_t grp_lid, uint8_t sgrp_lid, bes_ble_bis_src_metadata_update_t *metadata)
{
    aob_bis_src_update_metadata(grp_lid, sgrp_lid, (app_gaf_bap_cfg_metadata_t *)metadata);
}

void bes_ble_bis_src_write_bis_data(uint8_t big_idx, uint8_t stream_lid, uint8_t *data, uint16_t data_len)
{
    aob_bis_src_write_bis_data(big_idx, stream_lid, data, data_len);
}

uint32_t bes_ble_bis_src_get_stream_anchor_time(uint8_t big_idx, uint8_t stream_lid)
{
    return aob_bis_src_get_anchor_time(big_idx, stream_lid);
}

void bes_ble_bis_src_start(uint8_t big_idx, bes_ble_bis_src_start_param_t *event_info)
{
    aob_bis_src_start(big_idx, (aob_bis_src_started_info_t *)event_info);
}

void bes_ble_bis_src_stop(uint8_t big_idx)
{
    aob_bis_src_stop(big_idx);
}

// bis sink
void bes_ble_bis_sink_start(bes_ble_bis_sink_start_param_t *param)
{
    aob_bis_sink_start((aob_bis_sink_start_param_t *)param);
}

void bes_ble_bis_sink_stop()
{
    aob_bis_sink_stop();
}

void bes_ble_bap_start_discovery(uint8_t con_lid)
{
    app_bap_start(con_lid);
}

void bes_ble_start_gaf_discovery(uint8_t con_lid)
{
    app_gaf_mobile_start_discovery(con_lid);
}

gaf_bap_activity_type_e bes_ble_bap_get_actv_type(void)
{
    return app_bap_get_activity_type();
}

#ifdef AOB_MOBILE_ENABLED
uint8_t bes_ble_bap_get_device_num_to_be_connected(void)
{
    return app_bap_get_device_num_to_be_connected();
}
#endif /// AOB_MOBILE_ENABLED

/*BAP ASCS API*/
const bes_ble_bap_ascs_ase_t *bes_ble_get_ascs_ase_info(uint8_t ase_lid)
{
    CHECK_SIZE_TYPE(bes_ble_bap_ascs_ase_t, sizeof(app_bap_ascs_ase_t));

    return (bes_ble_bap_ascs_ase_t *)app_bap_uc_srv_get_ase_info(ase_lid);
}

uint8_t bes_ble_bap_ascs_get_streaming_ase_lid_list(uint8_t con_lid, uint8_t *ase_lid_list)
{
    return aob_media_get_curr_streaming_ase_lid_list(con_lid, ase_lid_list);
}

void bes_ble_bap_ascs_send_ase_enable_rsp(uint8_t ase_lid, bool accept)
{
    aob_media_send_enable_rsp(ase_lid, accept);
}

void bes_ble_bap_ascs_disable_ase_req(uint8_t ase_lid)
{
    aob_media_disable_stream(ase_lid);
}

void bes_ble_bap_ascs_release_ase_req(uint8_t ase_lid)
{
    aob_media_release_stream(ase_lid);
}

uint32_t bes_ble_bap_capa_get_location_bf(bes_gaf_direction_t direction)
{
    return aob_pacs_get_cur_audio_location(direction);
}

void bes_ble_bap_dp_itf_data_come_callback_register(void *callback)
{
    app_bap_dp_itf_data_come_callback_register(callback);
}

void bes_ble_bap_dp_itf_data_come_callback_deregister(void)
{
    app_bap_dp_itf_data_come_callback_deregister();
}

uint8_t bes_ble_audio_get_tws_nv_role(void)
{
    return ble_audio_get_tws_nv_role();
}

uint8_t bes_ble_audio_get_location_fs_l_r_cnt(uint32_t audio_location_bf)
{
    return app_bap_get_audio_location_l_r_cnt(audio_location_bf);
}

uint8_t bes_ble_arc_get_mic_state(uint8_t con_lid)
{
    return aob_media_get_mic_state(con_lid);
}

void bes_ble_bap_iso_dp_send_data(uint16_t conhdl, uint16_t seq_num, uint8_t *payload, uint16_t payload_len, uint32_t ref_time)
{
    app_bap_dp_itf_send_data(conhdl, seq_num, payload, payload_len, ref_time);
}

uint8_t bes_ble_arc_convert_le_vol_to_local_vol(uint8_t le_vol)
{
    return aob_convert_le_vol_to_local_vol(le_vol);
}

uint8_t bes_ble_arc_vol_get_real_time_volume(uint8_t con_lid)
{
    return aob_vol_get_real_time_volume(con_lid);
}

int bes_ble_bap_get_free_iso_packet_num(void)
{
    return app_bap_get_free_packet_num();
}

void *bes_ble_bap_dp_itf_get_rx_data(uint16_t iso_hdl, bes_ble_dp_itf_iso_buffer_t *p_iso_buffer)
{
    return app_bap_dp_itf_get_rx_data(iso_hdl, (dp_itf_iso_buffer_t *)p_iso_buffer);
}

void bes_ble_bap_dp_tx_iso_stop(uint16_t iso_hdl)
{
    app_bap_dp_tx_iso_stop(iso_hdl);
}

void bes_ble_bap_dp_rx_iso_stop(uint16_t iso_hdl)
{
    app_bap_dp_rx_iso_stop(iso_hdl);
}

bool bes_ble_ccp_call_is_device_call_active(uint8_t con_lid)
{
    return aob_call_is_device_call_active(con_lid);
}

#ifdef AOB_MOBILE_ENABLED
const bes_ble_bap_ascc_ase_t *bes_ble_bap_ascc_get_ase_info(uint8_t ase_lid)
{
    CHECK_SIZE_TYPE(bes_ble_bap_ascc_ase_t, sizeof(app_bap_ascc_ase_t));

    return (bes_ble_bap_ascc_ase_t *)app_bap_uc_cli_get_ase_info_by_ase_lid(ase_lid);
}

uint8_t bes_ble_bap_ascc_get_specific_state_ase_lid_list(uint8_t con_lid, uint8_t direction, uint8_t ase_state, uint8_t *ase_lid_list)
{
    return app_bap_uc_cli_get_specific_state_ase_lid_list(con_lid, direction, ase_state, ase_lid_list);
}

bool bes_ble_bap_pacc_is_peer_support_stereo_channel(uint8_t con_lid, uint8_t direction)
{
    return app_bap_capa_cli_is_peer_support_stereo_channel(con_lid, direction);
}

void bes_ble_bap_ascc_configure_codec_by_ase_lid(uint8_t ase_lid, uint8_t cis_id, const AOB_CODEC_ID_T *codec_id,
                                                        uint16_t sampleRate, uint16_t frame_octet)
{
    app_bap_uc_cli_configure_codec(ase_lid, cis_id, (app_gaf_codec_id_t *)codec_id, sampleRate, frame_octet);
}

void bes_ble_bap_ascc_ase_release_by_ase_lid(uint8_t ase_lid)
{
    app_bap_uc_cli_stream_release(ase_lid);
}

void bes_ble_bap_ascc_ase_disable_by_ase_lid(uint8_t ase_lid)
{
    app_bap_uc_cli_stream_disable(ase_lid);
}

void bes_ble_bap_ascc_link_create_group_req(uint8_t cig_lid)
{
    app_bap_uc_cli_link_create_group_req(cig_lid);
}

void bes_ble_bap_ascc_link_remove_group_req(uint8_t grp_lid)
{
    app_bap_uc_cli_link_remove_group_cmd(grp_lid);
}

void bes_ble_bap_ascc_ase_qos_cfg_by_ase_lid(uint8_t ase_lid, uint8_t grp_lid)
{
    app_bap_uc_cli_configure_qos(ase_lid, grp_lid);
}

void bes_ble_bap_ascc_ase_enable_by_ase_lid(uint8_t ase_lid, uint16_t context_bf)
{
    app_bap_uc_cli_enable_stream(ase_lid, context_bf);
}

void bes_ble_mcp_mcs_action_control(uint8_t media_lid, uint8_t action)
{
    aob_media_mobile_action_control(media_lid, action);
}

void bes_ble_mcp_mcs_track_changed(uint8_t media_lid, uint32_t duration_10ms, uint8_t *title, uint8_t title_len)
{
    aob_media_mobile_change_track(media_lid, duration_10ms, title, title_len);
}
#endif /// AOB_MOBILE_ENABLED
#endif /* BLE_AUDIO_ENABLED */

#if (GATT_RATE_TESTS)
void bes_ble_rate_test_server_send_data_via_notification(uint8_t* ptrData, uint32_t length)
{
    app_rate_test_server_send_data_via_notification(ptrData, length);
}
void bes_ble_rate_test_server_send_data_via_intification(uint8_t* ptrData, uint32_t length)
{
    app_rate_test_server_send_data_via_indication(ptrData, length);
}
void bes_ble_rate_test_server_register_connected_done(bes_ble_rate_test_server_connected_done_t callback)
{
    app_rate_test_server_register_connected_done(callback);
}

void bes_ble_rate_test_server_register_conn_param_update_done(bes_ble_rate_test_server_conn_param_update_done_t callback)
{
    app_rate_test_server_register_conn_param_update_done(callback);
}
void bes_ble_rate_test_server_register_tx_done(bes_ble_rate_test_server_tx_done_t callback)
{
    app_rate_test_server_register_tx_done(callback);
}


#endif /* GATT_RATE_TESTS */

#if (GATT_RATE_TESTC)
void bes_ble_rate_test_client_register_connected_done(bes_ble_rate_test_client_connected_done_t callback)
{
    app_rate_test_client_register_connected_done(callback);
}

void bes_ble_rate_test_client_discover(uint8_t conidx)
{
    app_rate_test_client_discover(conidx);
}

void bes_ble_rate_test_client_register_conn_param_update_done(bes_ble_rate_test_client_conn_param_update_done_t callback)
{
    app_rate_test_client_register_conn_param_update_done(callback);
}

void bes_ble_rate_test_client_register_tx_done(bes_ble_rate_test_client_tx_done_t callback)
{
    app_rate_test_client_register_tx_done(callback);
}

void bes_ble_rate_test_client_send_data_via_write_command(uint8_t* ptrData, uint32_t length)
{
    app_rate_test_client_send_data_via_write_command(ptrData, length);
}

#endif /* GATT_RATE_TESTC */

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
#ifndef __BLE_TWS_H__
#define __BLE_TWS_H__
#include "bt_common_define.h"
#ifdef __cplusplus
extern "C"{
#endif

typedef void (*app_ble_tws_sync_volume_callback)(void);
typedef void (*app_ble_tws_sync_volume_rec_callback)(uint8_t *buf, uint8_t buf_len);
typedef bool (*app_ble_tws_is_connected_t)(void);
typedef void (*app_ble_tws_cmd_send_via_ble_t)(uint8_t*, uint16_t);

// sync@APP_TWS_IBRT_MAX_DATA_SIZE
#define BLE_TWS_SYNC_MAX_DATA_SIZE  (672)

typedef enum
{
    BLE_TWS_SYNC_EXCH_BLE_AUDIO_INFO = 0,
    BLE_TWS_SYNC_SYNC_DEV_INFO,
    BLE_TWS_SYNC_SHARE_SERVICE_INFO,
    //new customer cmd add here
} APP_BLE_TWS_SYNC_CMD_CODE_E;

void ble_audio_tws_sync_init(void);

int app_ble_tws_sync_send_cmd(APP_BLE_TWS_SYNC_CMD_CODE_E code, uint8_t high_priority, uint8_t *data, uint16_t data_len);
int app_ble_tws_sync_send_rsp(APP_BLE_TWS_SYNC_CMD_CODE_E code, uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void app_ble_tws_sync_sniff_manager(uint8_t con_lid, bool streaming);

bool app_ble_tws_sync_volume(uint8_t *data, uint8_t data_len);
bool app_ble_tws_sync_volume_register(app_ble_tws_sync_volume_rec_callback receive_cb,
                                                   app_ble_tws_sync_volume_callback trigger_cb, 
                                                   app_ble_tws_sync_volume_callback offset_trigger_cb);

int app_ble_tws_sync_release_cmd_semphore(void);
int app_ble_tws_sync_send_cmd_via_ble_register(app_ble_tws_cmd_send_via_ble_t cb);

#ifdef __cplusplus
}
#endif
#endif /*__BLE_TWS_H__*/

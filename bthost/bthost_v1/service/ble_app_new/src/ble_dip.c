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
#if defined(BLE_DIP_ENABLE)
#include "gatt_service.h"

#define MANUFACTURER_NAME "DEMO"
#define FIRMWARE_REVISION_STRING "0000"

#define PNP_VID_SOURCE  0x01
#define PNP_VENDOR_ID   0x02B0
#define PNP_PRODUCT_ID  0x0000
#define PNP_PRODUCT_VER 0x001F

typedef struct {
    uint8_t vendor_id_source; // 0x01 bluetooth sig
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t product_version;
} __attribute__ ((packed)) ble_dip_pnp_info_t;

GATT_DECL_PRI_SERVICE(g_ble_dip_service, GATT_UUID_DIP_SERVICE);

GATT_DECL_CHAR(g_ble_dip_manufacturer_name,
    GATT_CHAR_UUID_MANUFACTURER_NAME_STRING,
    GATT_RD_REQ,
    ATT_SEC_NONE);

GATT_DECL_CHAR(g_ble_dip_firmware_revision,
    GATT_CHAR_UUID_FW_REVISION_STRING,
    GATT_RD_REQ,
    ATT_SEC_NONE);

GATT_DECL_CHAR(g_ble_dip_pnp_info,
    GATT_CHAR_UUID_PNP_ID,
    GATT_RD_REQ,
    ATT_SEC_NONE);

static const gatt_attribute_t g_ble_dip_service_attr_list[] = {
    /* Service */
    gatt_attribute(g_ble_dip_service),
    /* Characteristics */
    gatt_attribute(g_ble_dip_manufacturer_name),
    gatt_attribute(g_ble_dip_firmware_revision),
    gatt_attribute(g_ble_dip_pnp_info),
};

static int app_ble_dip_server_callback(gatt_svc_t *svc, gatt_server_event_t event, gatt_server_callback_param_t param)
{
    switch (event)
    {
        case GATT_SERV_EVENT_CHAR_READ:
        {
            gatt_server_char_read_t *p = param.char_read;
            const uint8_t *c = p->character;
            if (c == g_ble_dip_manufacturer_name)
            {
                char manufacturer_name[] = {MANUFACTURER_NAME};
                gatts_write_read_rsp_data(p->ctx, (uint8_t *)manufacturer_name, strlen(manufacturer_name));
                return true;
            }
            else if (c == g_ble_dip_firmware_revision)
            {
                char fw_revision[] = {FIRMWARE_REVISION_STRING};
                gatts_write_read_rsp_data(p->ctx, (uint8_t *)fw_revision, strlen(fw_revision));
                return true;
            }
            else if (c == g_ble_dip_pnp_info)
            {
                ble_dip_pnp_info_t pnp_info;
                pnp_info.vendor_id_source = PNP_VID_SOURCE;
                pnp_info.vendor_id = co_host_to_uint16_le(PNP_VENDOR_ID);
                pnp_info.product_id = co_host_to_uint16_le(PNP_PRODUCT_ID);
                pnp_info.product_version = co_host_to_uint16_le(PNP_PRODUCT_VER);
                gatts_write_read_rsp_data(p->ctx, (uint8_t *)&pnp_info, sizeof(pnp_info));
                return true;
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

void ble_dips_init(void)
{
    gatts_register_service(g_ble_dip_service_attr_list, ARRAY_SIZE(g_ble_dip_service_attr_list), app_ble_dip_server_callback, NULL);
}

#endif /* BLE_DIP_ENABLE */

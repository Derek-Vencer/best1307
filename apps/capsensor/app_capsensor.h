/***************************************************************************
 *
 * Copyright 2022-2023 BES.
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

#ifndef _APP_CAPSENSOR_H_
#define _APP_CAPSENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif
#ifdef CUSTOMER_APP_BOAT
#include "plat_types.h"
void app_inear_detect_set_on_ear(void);
void app_inear_detect_set_off_ear(void);
bool app_inear_get_status(void);
#endif

#include "stdint.h"

void app_mcu_core_capsensor_init(void);

uint8_t app_capsensor_ch_num_get(void);

uint8_t app_capsensor_conversion_num_get(void);

uint16_t app_capsensor_samp_fs_get(void);

uint8_t app_capsensor_ch_map_get(void);

void app_capsensor_suspend(void);

void app_capsensor_resume(void);

void app_capsensor_close(void);

void app_capsensor_open(void);

void app_capsensor_driver_baseline_dr(uint32_t* baseline_value_p);

void app_capsensor_driver_baseline_reg_read(uint32_t* baseline_value_p);

#ifdef __cplusplus
}
#endif

#endif /* _APP_CAPSENSOR_H_ */

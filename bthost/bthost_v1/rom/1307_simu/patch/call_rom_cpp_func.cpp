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
#include <stdint.h>

extern struct avdtp_control_t *_a2dp_search_avdtp_ctl_from_id(uint8_t device_id, bool is_channel_req_context);
extern struct a2dp_control_t *_a2dp_get_control_from_id(uint8_t device_id);

extern "C" struct avdtp_control_t *call_a2dp_search_avdtp_ctl_from_id(uint8_t device_id, bool is_channel_req_context);
extern "C" struct a2dp_control_t *call_a2dp_get_control_from_id(uint8_t device_id);

struct avdtp_control_t *call_a2dp_search_avdtp_ctl_from_id(uint8_t device_id, bool is_channel_req_context)
{
    return _a2dp_search_avdtp_ctl_from_id(device_id, is_channel_req_context);
}

struct a2dp_control_t *call_a2dp_get_control_from_id(uint8_t device_id)
{
    return _a2dp_get_control_from_id(device_id);
}

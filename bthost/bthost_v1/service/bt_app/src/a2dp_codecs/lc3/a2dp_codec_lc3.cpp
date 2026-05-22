/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifdef BT_A2DP_SUPPORT
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
#include "app_audio.h"
#include "bluetooth.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"
#include "hal_location.h"
#include "a2dp_api.h"
#if defined(NEW_NV_RECORD_ENABLED)
#include "nvrecord_bt.h"
#endif

#include "a2dp_api.h"
#include "avrcp_api.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "app_bt.h"
#include "apps.h"
#include "app_bt_media_manager.h"
#include "tgt_hardware.h"
#include "bt_drv_interface.h"
#include "hci_api.h"

#include "a2dp_codec_lc3.h"

extern struct BT_DEVICE_T  app_bt_device;

#if defined(A2DP_LC3_ON)
static btif_avdtp_codec_t a2dp_lc3_avdtpcodec;

#if defined(A2DP_LC3_HR)
const unsigned char a2dp_codec_lc3_elements[A2DP_LC3_OCTET_NUMBER] = {
    0xA9, 0x08, 0x00, 0x00, //Vendor ID 0x0000038F  0x000008A9
    0x01, 0x00,         //Codec ID : LC3:0x8FAA, LC3 plus:0x8FAB LC3 plus:0x001
    (A2DP_LC3_FRAME_LEN_10MS | A2DP_LC3_FRAME_LEN_5MS | A2DP_LC3_FRAME_LEN_2POINT5MS),
    (A2DP_LC3_CH_MD_MONO | A2DP_LC3_CH_MD_STEREO),
    (A2DP_LC3_SR_48000),
    (A2DP_LC3_SR_96000),
    (A2DP_LC3_FMT_16BIT | A2DP_LC3_FMT_24BIT | A2DP_LC3_FMT_32BIT),
};
#else
const unsigned char a2dp_codec_lc3_elements[A2DP_LC3_OCTET_NUMBER] = {
    0x8F, 0x03, 0x00, 0x00, //Vendor ID 0x0000038F
    0xAA, 0x8F,         //Codec ID : LC3:0x8FAA, LC3 plus:0x8FAB
    (A2DP_LC3_SR_96000 | A2DP_LC3_SR_48000 | A2DP_LC3_SR_44100 | A2DP_LC3_SR_32000 | 
        A2DP_LC3_SR_24000 | A2DP_LC3_SR_16000| A2DP_LC3_SR_8000),
    (A2DP_LC3_FMT_16BIT | A2DP_LC3_FMT_24BIT | A2DP_LC3_FMT_32BIT | 
        A2DP_LC3_FRAME_LEN_10MS | A2DP_LC3_FRAME_LEN_7POINT5MS| A2DP_LC3_FRAME_LEN_5MS | A2DP_LC3_FRAME_LEN_2POINT5MS),
    (A2DP_LC3_BITRATE_900kBPS | A2DP_LC3_BITRATE_600kBPS | A2DP_LC3_BITRATE_500kBPS | A2DP_LC3_BITRATE_400kBPS | 
        A2DP_LC3_BITRATE_300kBPS | A2DP_LC3_BITRATE_200kBPS |A2DP_LC3_BITRATE_100kBPS  | A2DP_LC3_BITRATE_64kBPS),
    (A2DP_LC3_CH_MD_MONO | A2DP_LC3_CH_MD_STEREO | A2DP_LC3_CH_MD_MUlTI_MONO | A2DP_LC3_BR_MODE_AUTO),
};
#endif    

void a2dp_codec_lc3_common_init(void)
{
    TRACE(1, "%s", __func__);
    a2dp_lc3_avdtpcodec.codecType = BT_A2DP_CODEC_TYPE_NON_A2DP;
    a2dp_lc3_avdtpcodec.discoverable = 1;
    a2dp_lc3_avdtpcodec.elements = (U8 *)&a2dp_codec_lc3_elements;
    a2dp_lc3_avdtpcodec.elemLen  = sizeof(a2dp_codec_lc3_elements);
    {
        btif_avdtp_codec_t *p = &a2dp_lc3_avdtpcodec;
        TRACE(1,"a2dp_lc3_avdtpcodec.elemLen = %d \n", p->elemLen);

        TRACE(5,"a2dp_lc3_avdtpcodec.elements->[0]=0x%02x,[1]=0x%02x,[2]=0x%02x,[3]=0x%02x,[4]=0x%02x\n",
                                    p->elements[0], p->elements[1], p->elements[2],
                                    p->elements[3], p->elements[4]);

        TRACE(5,"a2dp_lc3_avdtpcodec.elements->[5]=0x%02x,[6]=0x%02x,[7]=0x%02x,[8]=0x%02x,[9]=0x%02x\n",
                                    p->elements[5], p->elements[6], p->elements[7],
                                    p->elements[8], p->elements[9]);
    }
}

bt_status_t a2dp_codec_lc3_init(int index)
{
   bt_status_t st;
   struct BT_DEVICE_T *curr_device = app_bt_get_device(index);

    TRACE(1, "%s", __func__);
    a2dp_codec_lc3_common_init();

    st = btif_a2dp_register(curr_device->btif_a2dp_stream, BTIF_A2DP_STREAM_TYPE_SINK, &a2dp_lc3_avdtpcodec, NULL,0,a2dp_callback);

    return st;
}
#endif /* A2DP_LC3_ON */
#endif /* BT_A2DP_SUPPORT */
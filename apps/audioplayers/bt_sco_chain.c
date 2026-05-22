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
#include "plat_types.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_codec.h"
#include "hal_aud.h"
#include "audio_dump.h"
#include "speech_cfg.h"
#include "bt_sco_chain.h"
#include "bt_sco_chain_cfg.h"
#include "bt_sco_chain_tuning.h"
#include "hal_overlay.h"
#include "mpu.h"

#include "app_utils.h"
#include "arm_math_ex.h"

#if defined(SPEECH_ALGO_DSP)
#include "bt_sco_chain_dsp.h"
#endif

#if defined(SPEECH_TX_24BIT)
typedef int     SPEECH_PCM_T;
#else
typedef short   SPEECH_PCM_T;
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
#if defined(SPEECH_ROM_PATCH_9_P_1) || defined(SPEECH_ROM_PATCH_9_P_5)
#include "patch.h"
#include "hal_location.h"
extern const unsigned char besnet_weights[];
#endif
#endif

#define SCO_CP_ACCEL_ALGO_START()
#define SCO_CP_ACCEL_ALGO_END()

#define SCO_CP_ACCEL_ALGO_INIT_START()
#define SCO_CP_ACCEL_ALGO_INIT_END()

#define SPEECH_TX_AEC2FLOAT_CORE 0
#define SPEECH_TX_NS2FLOAT_CORE 0
#define SPEECH_RX_NS2FLOAT_CORE 0


//#define BT_SCO_CHAIN_PROFILE
// #define BT_SCO_CHAIN_AUDIO_DUMP
// #define TWS_SWITCH_ACCORDING_NOISE
#define BT_SCO_LOW_RAM
//#define SPEECH_TX_3MIC_SWAP_CHANNELS

#if defined(TWS_SWITCH_ACCORDING_NOISE)
#include "app_tws_ibrt_cmd_handler.h"
#endif

/* #define SPEECH_DC_FILTER_RPC */
#if defined(SPEECH_DC_FILTER_RPC)
#include "speech_dc_filter_frontend.h"
#endif

#if defined(BT_SCO_CHAIN_PROFILE)
static uint32_t tx_start_ticks = 0;
static uint32_t tx_end_ticks = 0;
#endif

extern const SpeechConfig speech_cfg_default;
static SpeechConfig *speech_cfg = NULL;

FrameResizeState *speech_frame_resize_st = NULL;

#if defined(SPEECH_TX_24BIT)
int32_t *aec_echo_buf = NULL;
#else
short *aec_echo_buf = NULL;
#endif
short *aec_out_buf = NULL;

#if defined(SPEECH_TX_2MIC_NS8)
short *af_out_buf = NULL;
#endif

#if defined(SPEECH_BONE_SENSOR)
#include "speech_bone_sensor.h"
static SPEECH_PCM_T *vpu_buf = NULL;
#endif

// Use to free buffer
#if defined(SPEECH_TX_24BIT)
static int32_t *aec_echo_buf_ptr;
#else
static short *aec_echo_buf_ptr;
#endif

/*--------------------TX state--------------------*/
SpeechDcFilterState *speech_tx_dc_filter_st = NULL;

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
static Ec2FloatState *speech_tx_aec2float_st = NULL;
#endif

#if defined(SPEECH_TX_1MIC_NS)
Speech1MicNSState *speech_tx_1mic_ns_st = NULL;
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
SpeechNs9State *speech_tx_ns9_st = NULL;
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
Speech2MicPreafState *speech_tx_2mic_preaf_st = NULL;
#endif

#if defined(SPEECH_TX_2MIC_NS8)
Speech2MicNs8State *speech_tx_2mic_ns8_st = NULL;
#endif

// Gain
static CompexpState *speech_tx_compexp_st = NULL;

// EQ
EqState *speech_tx_eq_st = NULL;

// GAIN
#if defined(SPEECH_TX_POST_GAIN)
SpeechGainState *speech_tx_post_gain_st = NULL;
#endif

/*--------------------RX state--------------------*/

#if defined(SPEECH_RX_NS2FLOAT)
SpeechNs2FloatState *speech_rx_ns2float_st = NULL;
#endif

#if defined(SPEECH_RX_COMPEXP)
static MultiCompexpState *speech_rx_compexp_st = NULL;
#endif

// EQ
#if defined(SPEECH_RX_EQ)
EqState *speech_rx_eq_st = NULL;
#endif


static bool dualmic_enable = true;

void switch_dualmic_status(void)
{
    TRACE(3,"[%s] dualmic status: %d -> %d", __FUNCTION__, dualmic_enable, !dualmic_enable);
    dualmic_enable ^= true;
}

static int speech_tx_sample_rate = 16000;
static int speech_rx_sample_rate = 16000;
static int speech_tx_frame_len = 256;
static int speech_rx_frame_len = 256;
static int speech_sco_frame_len = 240;

#ifndef SCO_CP_ACCEL_FIFO
static bool speech_tx_frame_resizer_enable = false;
static bool speech_rx_frame_resizer_enable = false;
#endif

extern bool bt_sco_codec_is_cvsd(void);

static int32_t _speech_tx_process_(void *pcm_buf, void *ref_buf, int32_t *pcm_len);
static int32_t _speech_rx_process_(void *pcm_buf, int32_t *pcm_len);
enum APP_SYSFREQ_FREQ_T speech_get_proper_sysfreq(int *needed_mips);

void *speech_get_ext_buff(int size)
{
    void *pBuff = NULL;
    if (size % 4)
    {
        size = size + (4 - size % 4);
    }

    pBuff = speech_calloc(size, sizeof(uint8_t));
    TRACE(2,"[%s] len:%d", __func__, size);

    return pBuff;
}

int speech_store_config(const SpeechConfig *cfg)
{
    if (speech_cfg)
    {
        memcpy(speech_cfg, cfg, sizeof(SpeechConfig));
    }
    else
    {
        TRACE(1,"[%s] WARNING: Please phone call...", __func__);
    }

    return 0;
}

#define HARDWARE_ECHO_DELAY (70)
static int echo_delay = HARDWARE_ECHO_DELAY;

int _speech_tx_init_pre(int sample_rate, int frame_len)
{
    TRACE(3,"[%s] Start, sample_rate: %d, frame_len: %d", __func__, sample_rate, frame_len);

    int channel_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
    int data_separation = 0;

    speech_tx_dc_filter_st = speech_dc_filter_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_dc_filter, default_allocator());
    speech_dc_filter_ctl(speech_tx_dc_filter_st, SPEECH_DC_FILTER_SET_CHANNEL_NUM, &channel_num);
    speech_dc_filter_ctl(speech_tx_dc_filter_st, SPEECH_DC_FILTER_SET_DATA_SEPARATION, &data_separation);

    echo_delay = HARDWARE_ECHO_DELAY;

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
#if defined(SPEECH_ROM_PATCH_9_P_1) || defined(SPEECH_ROM_PATCH_9_P_5)
    patch_open(PATCH_CTRL_ID_0, 0);
    //model_init
    patch_enable(PATCH_CTRL_ID_0, PATCH_TYPE_DATA, 0x22024580, (uint32_t)&besnet_weights);
    patch_enable(PATCH_CTRL_ID_0, PATCH_TYPE_DATA, 0x22024908, (uint32_t)&besnet_weights);
#endif
#endif

#if defined(SPEECH_TX_1MIC_NS)
    void *model_data = speech_1mic_ns_get_model_data_94l();
    speech_tx_1mic_ns_st = speech_1mic_ns_create(model_data, &speech_cfg->tx_1mic_ns, default_allocator());
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    speech_tx_ns9_st = speech_ns9_create(sample_rate, frame_len, &speech_cfg->tx_ns9, default_allocator());
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_tx_2mic_preaf_st = speech_2mic_preaf_create(sample_rate, frame_len, &speech_cfg->tx_2mic_preaf, default_allocator());
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    void *model_data_s = speech_2mic_ns8_get_model_data_95s();
    speech_tx_2mic_ns8_st = speech_2mic_ns8_create(model_data_s, sample_rate, frame_len, &speech_cfg->tx_2mic_ns8, default_allocator());
    echo_delay += speech_2mic_ns8_get_delay(speech_tx_2mic_ns8_st);
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    af_out_buf = (short *)speech_calloc(frame_len * AEC_OUT_BUF_CHAN_NUM, sizeof(short));
#endif

// #if !(defined(__AUDIO_RESAMPLE__) && defined(SW_SCO_RESAMPLE))
    aec_out_buf = (short *)speech_calloc(frame_len, sizeof(short));
#if defined(SPEECH_TX_24BIT)
    aec_echo_buf = (int32_t *)speech_calloc(frame_len, sizeof(int32_t));
#else
    aec_echo_buf = (short *)speech_calloc(frame_len, sizeof(short));
#endif
    aec_echo_buf_ptr = aec_echo_buf;
// #endif

    TRACE(2, "[%s] echo delay is %d", __FUNCTION__, echo_delay);

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    speech_tx_aec2float_st = ec2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_TX_AEC2FLOAT_CORE, &speech_cfg->tx_aec2float, default_allocator());
#endif

    speech_tx_compexp_st = compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_compexp, default_allocator());

    speech_tx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->tx_eq, default_allocator());

#if defined(SPEECH_TX_POST_GAIN)
    speech_tx_post_gain_st = speech_gain_create(sample_rate, frame_len, &speech_cfg->tx_post_gain);
#endif

    TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_tx_init(int sample_rate, int ap_frame_len, int cp_frame_len)
{
    _speech_tx_init_pre(sample_rate, ap_frame_len);

    return 0;
}

int speech_rx_init(int sample_rate, int frame_len)
{
    TRACE(3,"[%s] Start, sample_rate: %d, frame_len: %d", __func__, sample_rate, frame_len);

#if defined(SPEECH_RX_NS2FLOAT)
#ifdef BT_SCO_LOW_RAM
    if (16000 == sample_rate)
    {
        speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len/2, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
    }
    else
    {
        speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
    }
#else
    speech_rx_ns2float_st = speech_ns2float_create_with_custom_allocator(sample_rate, frame_len, SPEECH_RX_NS2FLOAT_CORE, &speech_cfg->rx_ns2float, default_allocator());
#endif
#endif


#if defined(SPEECH_RX_COMPEXP)
    speech_rx_compexp_st = multi_compexp_create_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_compexp, default_allocator());
#endif

#if defined(SPEECH_RX_EQ)
    speech_rx_eq_st = eq_init_with_custom_allocator(sample_rate, frame_len, &speech_cfg->rx_eq, default_allocator());
#endif


    TRACE(1,"[%s] End", __func__);

    return 0;
}

float speech_sysfreq_to_mips(enum HAL_CMU_FREQ_T sysfreq);

int speech_init2(int tx_sample_rate, int rx_sample_rate,
                     int tx_frame_len, int rx_frame_len,
                     int sco_frame_len,
                     uint8_t *buf, int len)
{
    TRACE(1,"[%s] Start...", __func__);

    //ASSERT(frame_ms == SPEECH_PROCESS_FRAME_MS, "[%s] frame_ms(%d) != SPEECH_PROCESS_FRAME_MS(%d)", __func__,
    //                                                                                                frame_ms,
    //                                                                                                SPEECH_PROCESS_FRAME_MS);

    speech_tx_sample_rate = tx_sample_rate;
    speech_rx_sample_rate = rx_sample_rate;

    speech_tx_frame_len = tx_frame_len;
    speech_rx_frame_len = rx_frame_len;
    speech_sco_frame_len = sco_frame_len;

    speech_heap_init(buf, len);
#if 1//defined(SCO_OPTIMIZE_FOR_RAM)
    enum HAL_OVERLAY_ID_T id = hal_overlay_get_curr_id();
    if(id != HAL_OVERLAY_ID_QTY){
        speech_heap_add_block((uint8_t *)hal_overlay_get_text_free_addr(id), hal_overlay_get_text_free_size(id));
    }
#endif
    // When phone call again, speech cfg will be default.
    // If do not want to reset speech cfg, need define bt_sco_chain_init()
    // and call in apps.cpp: app_init()
    speech_cfg = (SpeechConfig *)speech_calloc(1, sizeof(SpeechConfig));
    speech_store_config(&speech_cfg_default);

#ifdef AUDIO_DEBUG
    speech_tuning_open();
#endif

#ifndef SCO_CP_ACCEL_FIFO
    int aec_enable = 0;
    aec_enable = 1;

    int capture_sample_size = sizeof(int16_t), playback_sample_size = sizeof(int16_t);
#if defined(SPEECH_TX_24BIT)
    capture_sample_size = sizeof(int32_t);
#endif
#if defined(SPEECH_RX_24BIT)
    playback_sample_size = sizeof(int32_t);
#endif

    CAPTURE_HANDLER_T tx_handler = (tx_frame_len == sco_frame_len) ? NULL : _speech_tx_process_;
    PLAYBACK_HANDLER_T rx_handler = ((rx_sample_rate/rx_frame_len) == (tx_sample_rate/sco_frame_len)) ? NULL : _speech_rx_process_;

    speech_tx_frame_resizer_enable = (tx_handler != NULL);
    speech_rx_frame_resizer_enable = (rx_handler != NULL);

    if (speech_tx_frame_resizer_enable || speech_rx_frame_resizer_enable) {
        speech_frame_resize_st = frame_resize_create(speech_sco_frame_len,
                                                     SPEECH_CODEC_CAPTURE_CHANNEL_NUM,
                                                     speech_tx_frame_len,
                                                     capture_sample_size,
                                                     playback_sample_size,
                                                     aec_enable,
                                                     tx_handler,
                                                     rx_handler
                                                    );
    }
#endif

#if defined(SPEECH_BONE_SENSOR)
    vpu_buf = (SPEECH_PCM_T *)speech_calloc(speech_tx_frame_len, sizeof(SPEECH_PCM_T));
#endif


#if defined(SPEECH_ALGO_DSP)
    SCO_DSP_CFG_T dsp_cfg;
    memset(&dsp_cfg, 0, sizeof(dsp_cfg));
    dsp_cfg.sample_rate = speech_tx_sample_rate;
#if defined(SPEECH_TX_24BIT)
    dsp_cfg.sample_bytes= sizeof(int32_t);
#else
    dsp_cfg.sample_bytes= sizeof(int16_t);
#endif
    dsp_cfg.frame_len   = speech_tx_frame_len;
    dsp_cfg.mic_num     = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
    dsp_cfg.mode        = (bt_sco_codec_is_cvsd() == false);
    dsp_cfg.capture_enable  = true;
    dsp_cfg.playback_enable = true;
    dsp_cfg.user        = SPEECH_ALGO_DSP_USER_CALL;
    speech_algo_dsp_open(&dsp_cfg);
#endif

#if defined(SCO_CP_ACCEL_FIFO)
    speech_tx_init(speech_tx_sample_rate, speech_sco_frame_len, speech_tx_frame_len);
#else
    speech_tx_init(speech_tx_sample_rate, speech_tx_frame_len, speech_tx_frame_len);
#endif

    speech_rx_init(speech_rx_sample_rate, speech_rx_frame_len);

#if !defined(SCO_CP_ACCEL) && !defined(SPEECH_DC_FILTER_RPC)
    int needed_freq = 0;
    enum APP_SYSFREQ_FREQ_T min_system_freq = speech_get_proper_sysfreq(&needed_freq);

    enum APP_SYSFREQ_FREQ_T freq = hal_sysfreq_get();

    if (freq < min_system_freq) {
        freq = min_system_freq;

        if (freq > (int)HAL_CMU_FREQ_104M) {
            TRACE(2, "[%s] required mips %d(needed freq %d) is too large, keep 104M", __FUNCTION__, freq, needed_freq);
            freq = HAL_CMU_FREQ_104M;
        }

        app_sysfreq_req(APP_SYSFREQ_USER_BT_SCO, freq);
        TRACE(2,"[%s]: app_sysfreq_req %d", __FUNCTION__, freq);
    }
#endif
    //app_sysfreq_req(APP_SYSFREQ_USER_BT_SCO, APP_SYSFREQ_104M);

// DUMP: Multi MICs + Ref + VPU + OUT
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    uint32_t dump_ch = SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1;
    if (aec_enable) {
        dump_ch += 1;
    }
#if defined(SPEECH_BONE_SENSOR)
    dump_ch += 1;
#endif
    //audio_dump_init(speech_tx_frame_len, sizeof(short), dump_ch);
    audio_dump_init(speech_tx_frame_len, sizeof(short), 5);
#endif

    TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_init(int tx_sample_rate, int rx_sample_rate,
                     int tx_frame_ms, int rx_frame_ms,
                     int sco_frame_ms,
                     uint8_t *buf, int len)
{
    int tx_frame_len = SPEECH_FRAME_MS_TO_LEN(tx_sample_rate, tx_frame_ms);
    int rx_frame_len = SPEECH_FRAME_MS_TO_LEN(rx_sample_rate, rx_frame_ms);
    int sco_frame_len = SPEECH_FRAME_MS_TO_LEN(tx_sample_rate, sco_frame_ms);

    return speech_init2(tx_sample_rate, rx_sample_rate,
                        tx_frame_len, rx_frame_len,
                        sco_frame_len,
                        buf, len);
}

int speech_tx_deinit(void)
{
    TRACE(1,"[%s] Start...", __func__);

#if defined(SPEECH_TX_POST_GAIN)
    speech_gain_destroy(speech_tx_post_gain_st);
#endif

    eq_destroy(speech_tx_eq_st);

#if defined(SPEECH_TX_2MIC_NS8)
    speech_2mic_ns8_destroy(speech_tx_2mic_ns8_st);
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_destroy(speech_tx_2mic_preaf_st);
#endif

#if defined(SPEECH_TX_1MIC_NS)
    speech_1mic_ns_destroy(speech_tx_1mic_ns_st);
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    speech_ns9_destory(speech_tx_ns9_st);
#endif

    compexp_destroy(speech_tx_compexp_st);

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    ec2float_destroy(speech_tx_aec2float_st);
#endif
    speech_free(aec_echo_buf_ptr);
    speech_free(aec_out_buf);

#if defined(SPEECH_TX_2MIC_NS8)
    speech_free(af_out_buf);
#endif

    speech_dc_filter_destroy(speech_tx_dc_filter_st);

    TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_rx_deinit(void)
{
    TRACE(1,"[%s] Start...", __func__);

#if defined(SPEECH_RX_EQ)
    eq_destroy(speech_rx_eq_st);
#endif


#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_destroy(speech_rx_compexp_st);
#endif

#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_destroy(speech_rx_ns2float_st);
#endif

    TRACE(1,"[%s] End", __func__);

    return 0;
}

int speech_deinit(void)
{
    TRACE(1,"[%s] Start...", __func__);


#if defined(SPEECH_ALGO_DSP)
    speech_algo_dsp_close();
#endif

    speech_rx_deinit();
    speech_tx_deinit();

#if defined(SPEECH_BONE_SENSOR)
    speech_free(vpu_buf);
#endif

#ifndef SCO_CP_ACCEL_FIFO
    if (speech_frame_resize_st != NULL) {
        frame_resize_destroy(speech_frame_resize_st);
        speech_tx_frame_resizer_enable = false;
        speech_rx_frame_resizer_enable = false;
    }
#endif

#ifdef AUDIO_DEBUG
    speech_tuning_close();
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_deinit();
#endif

    speech_free(speech_cfg);
    speech_cfg = NULL;

    size_t total = 0, used = 0, max_used = 0;
    speech_memory_info(&total, &used, &max_used);
    TRACE(3,"SPEECH MALLOC MEM: total - %d, used - %d, max_used - %d.", total, used, max_used);
    ASSERT(used == 0, "[%s] used != 0", __func__);

    TRACE(1,"[%s] End", __func__);

    return 0;
}

float speech_tx_get_required_mips(void)
{
    float mips = 0;

    mips += speech_dc_filter_get_required_mips(speech_tx_dc_filter_st);

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    mips += ec2float_get_required_mips(speech_tx_aec2float_st);
#endif

    mips += compexp_get_required_mips(speech_tx_compexp_st);

    mips += eq_get_required_mips(speech_tx_eq_st);

#if defined(SPEECH_TX_POST_GAIN)
    mips += speech_gain_get_required_mips(speech_tx_post_gain_st);
#endif

    return mips;
}

float speech_rx_get_required_mips(void)
{
    float mips = 0;

#if defined(SPEECH_RX_NS2FLOAT)
    mips += speech_ns2float_get_required_mips(speech_rx_ns2float_st);
#endif

#if defined(SPEECH_RX_COMPEXP)
    mips += multi_compexp_get_required_mips(speech_rx_compexp_st);
#endif

#if defined(SPEECH_RX_EQ)
    mips += eq_get_required_mips(speech_rx_eq_st);
#endif

    return mips;
}

float speech_get_required_mips(void)
{
    return speech_tx_get_required_mips() + speech_rx_get_required_mips();
}

#define SYSTEM_BASE_MIPS (18)

enum APP_SYSFREQ_FREQ_T speech_get_proper_sysfreq(int *needed_mips)
{
    enum APP_SYSFREQ_FREQ_T freq = APP_SYSFREQ_32K;
    int required_mips = (int)ceilf(speech_get_required_mips() + SYSTEM_BASE_MIPS);

    if (required_mips >= 104)
        freq = APP_SYSFREQ_208M;
    else if (required_mips >= 78)
        freq = APP_SYSFREQ_104M;
    else if (required_mips >= 52)
        freq = APP_SYSFREQ_78M;
    else if (required_mips >= 26)
        freq = APP_SYSFREQ_52M;
    else
        freq = APP_SYSFREQ_26M;

    *needed_mips = required_mips;

    return freq;
}

int speech_set_config(const SpeechConfig *cfg)
{
    speech_dc_filter_set_config(speech_tx_dc_filter_st, &cfg->tx_dc_filter);

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    ec2float_set_config(speech_tx_aec2float_st, &cfg->tx_aec2float, SPEECH_TX_AEC2FLOAT_CORE);
#endif

    compexp_set_config(speech_tx_compexp_st, &cfg->tx_compexp);


    eq_set_config(speech_tx_eq_st, &cfg->tx_eq);

#if defined(SPEECH_RX_NS2FLOAT)
    speech_ns2float_set_config(speech_rx_ns2float_st, &cfg->rx_ns2float, SPEECH_RX_NS2FLOAT_CORE);
#endif

#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_set_config(speech_rx_compexp_st, &cfg->rx_compexp);
#endif

#if defined(SPEECH_RX_EQ)
    eq_set_config(speech_rx_eq_st, &cfg->rx_eq);
#endif
    // Add more process

    return 0;
}

void _speech_tx_process_pre(short *pcm_buf, short *ref_buf, int *_pcm_len, sco_status_t *status)
{
    int pcm_len = *_pcm_len;
    int POSSIBLY_UNUSED frame_len = pcm_len / SPEECH_CODEC_CAPTURE_CHANNEL_NUM;

#if defined(BT_SCO_CHAIN_PROFILE)
    tx_start_ticks = hal_fast_sys_timer_get();
#endif

    // TRACE(2,"[%s] pcm_len = %d", __func__, pcm_len);

#ifdef AUDIO_DEBUG
    if (speech_tuning_get_status())
    {
        speech_set_config(speech_cfg);

        speech_tuning_set_status(false);

        // If has MIPS problem, can move this block code into speech_rx_process or return directly
        // return 0
    }
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_clear_up();
    uint32_t POSSIBLY_UNUSED dump_index = 0;
#endif

// NOTE: If more case need this, remove MACRO control
#if defined(SPEECH_ALGO_DSP) || defined(SPEECH_BONE_SENSOR)
    ASSERT(frame_len == speech_tx_frame_len, "[%s] frame_len(%d) != speech_tx_frame_len(%d)", \
            __func__, frame_len, speech_tx_frame_len);
#endif

#if defined(SPEECH_BONE_SENSOR)
#if defined(SPEECH_TX_24BIT)
    speech_bone_sensor_get_data(vpu_buf, frame_len, SPEECH_BS_CHANNEL_X, 24);
#else
    speech_bone_sensor_get_data(vpu_buf, frame_len, SPEECH_BS_CHANNEL_X, 16);
#endif
#endif

#if defined(SPEECH_ALGO_DSP)
    sco_dsp_process(pcm_buf, ref_buf, (int *)_pcm_len);
    return;
#endif

#if defined(SPEECH_ALGO_DSP) || defined(SPEECH_ALGO_DSP_TEST)
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    dump_index = 0;
    for (uint32_t ch=0; ch<SPEECH_CODEC_CAPTURE_CHANNEL_NUM; ch++) {
        audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, pcm_buf, frame_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM, ch, 8);
    }
    if (ref_buf) {
        audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, ref_buf, frame_len, 1, 0, 8);
    }
#if defined(SPEECH_BONE_SENSOR)
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, vpu_buf, frame_len, 1, 0, 8);
#endif
#endif

#if defined(SPEECH_ALGO_DSP)
    SCO_DSP_TX_PCM_T pcm_cfg = {0};
#else
    speech_algo_dsp_pcm_t pcm_cfg = {0};
#endif
    memset(&pcm_cfg, 0, sizeof(pcm_cfg));

    pcm_cfg.mic = pcm_buf;
    pcm_cfg.ref = ref_buf;
#if defined(SPEECH_BONE_SENSOR)
    pcm_cfg.vpu = vpu_buf;
#else
    pcm_cfg.vpu = NULL;
#endif
    pcm_cfg.out = pcm_buf;

#if defined(SPEECH_ALGO_DSP)
    pcm_cfg.frame_len = frame_len;
    speech_algo_dsp_capture_process(&pcm_cfg);
#else
    pcm_cfg.len = frame_len;
    speech_algo_dsp_test_process(&pcm_cfg);
#endif

    *_pcm_len = frame_len;

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data_from_multi_channels_32bit_to_16bit(dump_index++, pcm_buf, frame_len, 1, 0, 8);
    //audio_dump_run();
#endif
    return;
#endif

#if defined(SPEECH_TX_24BIT)
    speech_dc_filter_process_int24(speech_tx_dc_filter_st, (int32_t *)pcm_buf, pcm_len);
#else
    speech_dc_filter_process(speech_tx_dc_filter_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_TX_24BIT)
    arm_q23_to_q15((int32_t *)ref_buf, (int16_t *)ref_buf, pcm_len / SPEECH_CODEC_CAPTURE_CHANNEL_NUM);

    arm_q23_to_q15((int32_t *)pcm_buf, (int16_t *)pcm_buf, pcm_len);
#endif

#if 0
    // Test MIC: Get one channel data
    // Enable this, should bypass SPEECH_TX_2MIC_NS and SPEECH_TX_2MIC_NS2
    for(uint32_t i=0; i<pcm_len/2; i++)
    {
        pcm_buf[i] = pcm_buf[2*i];      // Left channel
        // pcm_buf[i] = pcm_buf[2*i+1]; // Right channel
    }

    pcm_len >>= 1;
#endif

    //audio_dump_add_channel_data_from_multi_channels(0, pcm_buf, pcm_len / 3, 3, 0);
    //audio_dump_add_channel_data_from_multi_channels(1, pcm_buf, pcm_len / 3, 3, 1);
    //audio_dump_add_channel_data_from_multi_channels(2, pcm_buf, pcm_len / 3, 3, 2);
//#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data(0, pcm_buf, pcm_len);
    if (ref_buf) {
        audio_dump_add_channel_data(1, ref_buf, frame_len);
    }
#endif

#if defined(SPEECH_TX_1MIC_NS)
    speech_1mic_ns_process(speech_tx_1mic_ns_st, pcm_buf, ref_buf, pcm_len);
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    ec2float_process(speech_tx_aec2float_st, pcm_buf, ref_buf, pcm_len, aec_out_buf);
    speech_copy_int16(pcm_buf, aec_out_buf, pcm_len);
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    speech_2mic_preaf_process(speech_tx_2mic_preaf_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    speech_2mic_ns8_process(speech_tx_2mic_ns8_st, pcm_buf, ref_buf, pcm_len, af_out_buf);
    speech_copy_int16(pcm_buf, af_out_buf, pcm_len / 2);
    pcm_len >>= 1;
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data(2, pcm_buf, pcm_len);
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    speech_ns9_process(speech_tx_ns9_st, pcm_buf, ref_buf, pcm_len);
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data(3, pcm_buf, pcm_len);
#endif

    compexp_process(speech_tx_compexp_st, pcm_buf, pcm_len);

    eq_process(speech_tx_eq_st, pcm_buf, pcm_len);

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    audio_dump_add_channel_data(4, pcm_buf, pcm_len);
    audio_dump_run();
#endif

#if defined(SPEECH_TX_24BIT)
    arm_q15_to_q23((int16_t *)pcm_buf, (int32_t *)pcm_buf, pcm_len);
#endif

    *_pcm_len = pcm_len;

#if defined(BT_SCO_CHAIN_PROFILE)
    tx_end_ticks = hal_fast_sys_timer_get();
    TRACE(2,"[%s] takes %d us", __FUNCTION__,
          FAST_TICKS_TO_US(tx_end_ticks - tx_start_ticks));
#endif
}

static POSSIBLY_UNUSED NOINLINE void test_cp_crash(void)
{
    TRACE(1, "%s", __FUNCTION__);
}


int32_t _speech_tx_process_(void *pcm_buf, void *ref_buf, int32_t *_pcm_len)
{
    sco_status_t status = {
        .vad = true,
    };

    _speech_tx_process_pre(pcm_buf, ref_buf, (int *)_pcm_len, &status);

    return 0;
}

int32_t _speech_rx_process_(void *pcm_buf, int32_t *_pcm_len)
{
    int32_t pcm_len = *_pcm_len;

#if defined(SPEECH_ALGO_DSP)
    SCO_DSP_RX_PCM_T pcm_cfg;
    memset(&pcm_cfg, 0, sizeof(pcm_cfg));
    pcm_cfg.in = pcm_buf;
    pcm_cfg.out = pcm_buf;
    pcm_cfg.frame_len = pcm_len;
    speech_algo_dsp_playback_process(&pcm_cfg);
#endif

#if defined(BT_SCO_CHAIN_PROFILE)
    uint32_t start_ticks = hal_fast_sys_timer_get();
#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    // audio_dump_add_channel_data(0, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_24BIT)
#if defined(SPEECH_RX_NS2FLOAT)
#ifdef BT_HFP_SUPPORT
    extern bool btapp_hfp_is_call_active(void);
    extern bool btapp_hfp_is_pc_call_active(void);
    if (btapp_hfp_is_pc_call_active() || btapp_hfp_is_call_active())
    {
        speech_ns2float_set_ringtone_mode(speech_rx_ns2float_st, false);
    }
    else
#endif
    {
        speech_ns2float_set_ringtone_mode(speech_rx_ns2float_st, true);
    }

#ifdef BT_SCO_LOW_RAM
    int32_t *pcm_buf32 = (int32_t *)pcm_buf;
    if (16000 == speech_rx_sample_rate)
    {
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf32, pcm_len/2);
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf32+pcm_len/2, pcm_len/2);
    }
    else
    {
        speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf, pcm_len);
    }
#else
    speech_ns2float_process_int24(speech_rx_ns2float_st, pcm_buf, pcm_len);
#endif
#endif

#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_process_int24(speech_rx_compexp_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_EQ)
    eq_process_int24(speech_rx_eq_st, pcm_buf, pcm_len);
#endif

#else

#if defined(SPEECH_RX_NS2FLOAT)
    // FIXME
    int16_t *pcm_buf16 = (int16_t *)pcm_buf;
    for(int i=0; i<pcm_len; i++)
    {
        pcm_buf16[i] = (int16_t)(pcm_buf16[i] * 0.94);
    }
#ifdef BT_SCO_LOW_RAM
    if (16000 == speech_rx_sample_rate)
    {
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf16, pcm_len/2);
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf16+pcm_len/2, pcm_len/2);
    }
    else
    {
        speech_ns2float_process(speech_rx_ns2float_st, pcm_buf, pcm_len);
    }
#else
    speech_ns2float_process(speech_rx_ns2float_st, pcm_buf, pcm_len);
#endif
#endif


#if defined(SPEECH_RX_COMPEXP)
    multi_compexp_process(speech_rx_compexp_st, pcm_buf, pcm_len);
#endif

#if defined(SPEECH_RX_EQ)
    eq_process(speech_rx_eq_st, pcm_buf, pcm_len);
#endif

#endif

#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
    // audio_dump_add_channel_data(1, pcm_buf, pcm_len);
    // audio_dump_run();
#endif

    *_pcm_len = pcm_len;

#if defined(BT_SCO_CHAIN_PROFILE)
    uint32_t end_ticks = hal_fast_sys_timer_get();
    TRACE(2,"[%s] takes %d us", __FUNCTION__,
        FAST_TICKS_TO_US(end_ticks - start_ticks));
#endif

    return 0;
}

int speech_tx_process(void *pcm_buf, void *ref_buf, int *pcm_len)
{
#ifdef SCO_CP_ACCEL_FIFO
    _speech_tx_process_(pcm_buf, ref_buf, (int32_t *)pcm_len);
#else
    if (speech_tx_frame_resizer_enable == false) {
        _speech_tx_process_(pcm_buf, ref_buf, (int32_t *)pcm_len);
    } else {
        // MUST use (int32_t *)??????
        frame_resize_process_capture(speech_frame_resize_st, pcm_buf, ref_buf, (int32_t *)pcm_len);
    }
#endif

    return 0;
}

int speech_rx_process(void *pcm_buf, int *pcm_len)
{
#ifdef SCO_CP_ACCEL_FIFO
    _speech_rx_process_(pcm_buf, (int32_t *)pcm_len);
#else
    if (speech_rx_frame_resizer_enable == false) {
        _speech_rx_process_(pcm_buf, (int32_t *)pcm_len);
    } else {
        frame_resize_process_playback(speech_frame_resize_st, pcm_buf, (int32_t *)pcm_len);
    }
#endif

    return 0;
}

int speech_tx_process_audio_dump(void *pcm_buf, int *_pcm_len, int channel_num)
{
#if defined(BT_SCO_CHAIN_AUDIO_DUMP)
	//TRACE(1,"[%s] ...channel_num: %d", __func__, channel_num);
	// int pcm_len = *_pcm_len;

	// for(int i = 0; i < channel_num; i++) {
	// 	audio_dump_add_channel_data_from_multi_channels(i, pcm_buf, pcm_len/channel_num, channel_num, (i%2));
	// }
    // audio_dump_run();
#endif
    return 0;
}

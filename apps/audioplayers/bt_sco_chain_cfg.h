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
#ifndef __BT_SCO_CHAIN_CFG_H__
#define __BT_SCO_CHAIN_CFG_H__

#include "speech_cfg.h"

typedef struct {
    SpeechDcFilterConfig    tx_dc_filter;
    
    Ec2FloatConfig          tx_aec2float;

#if defined(SPEECH_TX_1MIC_NS)
    Speech1MicNSConfig     tx_1mic_ns;
#endif

#if !defined(SPEECH_TX_1MIC_NS) && !defined(SPEECH_TX_2MIC_NS8)
    SpeechNs9Config         tx_ns9;
#endif

#if defined(SPEECH_TX_2MIC_PREAF)
    Speech2MicPreafConfig     tx_2mic_preaf;
#endif

#if defined(SPEECH_TX_2MIC_NS8)
    Speech2MicNs8Config     tx_2mic_ns8;
#endif

    CompexpConfig           tx_compexp;

    EqConfig                tx_eq;

#if defined(SPEECH_RX_NS2FLOAT)
    SpeechNs2FloatConfig    rx_ns2float;
#endif

#if defined(SPEECH_RX_COMPEXP)
    MultiCompexpConfig      rx_compexp;
#endif

#if defined(SPEECH_RX_EQ)
    EqConfig                rx_eq;
#endif

    // Add more process
} SpeechConfig;

#endif





























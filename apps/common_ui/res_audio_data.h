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
#ifndef __RES_AUDIO_DATA_H__
#define __RES_AUDIO_DATA_H__

#ifdef MEDIA_PLAYER_SUPPORT

const uint8_t EN_POWER_ON [] = {

#include "res/en/SOUND_POWER_ON.txt"

};

const uint8_t EN_POWER_OFF [] = {

#include "res/en/SOUND_POWER_OFF.txt"

};

const uint8_t EN_SOUND_ZERO[] = {

};

const uint8_t EN_SOUND_ONE[] = {

};

const uint8_t EN_SOUND_TWO[] = {

};

const uint8_t EN_SOUND_THREE[] = {

};

const uint8_t EN_SOUND_FOUR[] = {

};

const uint8_t EN_SOUND_FIVE[] = {

};

const uint8_t EN_SOUND_SIX[] = {

};

const uint8_t EN_SOUND_SEVEN [] = {

};

const uint8_t EN_SOUND_EIGHT [] = {

};

const uint8_t EN_SOUND_PROMPT_ADAPTIVE_ANC [] = {
};

const uint8_t EN_SOUND_CUSTOM_LEAK_DETECT [] = {
};

const uint8_t EN_SOUND_NINE [] = {

};

const uint8_t EN_BT_PAIR_ENABLE[] = {
#include "res/en/SOUND_PAIR_ENABLE.txt"
};

const uint8_t EN_BT_PAIRING[] = {

};

const uint8_t EN_BT_PAIRING_FAIL[] = {
};

const uint8_t EN_BT_PAIRING_SUCCESS[] = {
};

const uint8_t EN_BT_REFUSE[] = {
};

const uint8_t EN_BT_OVER[] = {
};

const uint8_t EN_BT_ANSWER[] = {
};

const uint8_t EN_BT_HUNG_UP[] = {
};

const uint8_t EN_BT_CONNECTED [] = {
#include "res/en/SOUND_CONNECTED.txt"
};

const uint8_t EN_BT_DIS_CONNECT [] = {
#include "res/en/SOUND_DIS_CONNECT.txt"
};

const uint8_t EN_BT_INCOMING_CALL [] = {
#include "res/en/SOUND_INCOMING_CALL.txt"
};

const uint8_t EN_CHARGE_PLEASE[] = {
#include "res/en/SOUND_CHARGE_PLEASE.txt"
};

const uint8_t EN_CHARGE_FINISH[] = {
};

const uint8_t EN_LANGUAGE_SWITCH[] = {
};

const uint8_t EN_BT_WARNING[] = {
#include "res/en/SOUND_WARNING.txt"
};

const uint8_t EN_BT_ALEXA_START[] = {
};

const uint8_t EN_BT_ALEXA_STOP[] = {
};

const uint8_t EN_BT_GSOUND_MIC_OPEN[] = {
};

const uint8_t EN_BT_GSOUND_MIC_CLOSE[] = {
};

const uint8_t EN_BT_GSOUND_NC[] = {
};

#ifdef __INTERACTION__
const uint8_t EN_BT_FINDME[] = {
};
#endif

const uint8_t EN_BT_MUTE[] = {
#include "res/SOUND_MUTE.txt"
};

#endif
#endif /* __RES_AUDIO_DATA_H__ */

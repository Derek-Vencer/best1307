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
#ifndef __RES_AUDIO_DATA_CN_H
#define __RES_AUDIO_DATA_CN_H

const uint8_t CN_POWER_ON [] = {
#include "res/cn/SOUND_POWER_ON.txt"
};

const uint8_t CN_POWER_OFF [] = {
#include "res/cn/SOUND_POWER_OFF.txt"
};

const uint8_t CN_SOUND_ZERO[] = { 
};

const uint8_t CN_SOUND_ONE[] = { 
};

const uint8_t CN_SOUND_TWO[] = { 
};

const uint8_t CN_SOUND_THREE[] = { 
};

const uint8_t CN_SOUND_FOUR[] = { 
};

const uint8_t CN_SOUND_FIVE[] = { 
};

const uint8_t CN_SOUND_SIX[] = { 
};

const uint8_t CN_SOUND_SEVEN[] = { 
};

const uint8_t CN_SOUND_EIGHT[] = { 
};

const uint8_t CN_SOUND_NINE[] = { 
};

const uint8_t CN_BT_PAIR_ENABLE[] = {
#include "res/cn/SOUND_PAIR_ENABLE.txt"
};

const uint8_t CN_BT_PAIRING [] = {
};

const uint8_t CN_BT_PAIRING_FAIL[] = {
};

const uint8_t CN_BT_PAIRING_SUCCESS[] = { 
};

const uint8_t CN_BT_REFUSE[] = { 
};

const uint8_t CN_BT_OVER[] = { 
};

const uint8_t CN_BT_ANSWER[] = { 
};


const uint8_t CN_BT_HUNG_UP[] = { 
};

const uint8_t CN_BT_INCOMING_CALL [] = {
#include "res/cn/SOUND_INCOMING_CALL.txt"
};

const uint8_t CN_BT_CONNECTED [] = {
#include "res/cn/SOUND_CONNECTED.txt"    
};

const uint8_t CN_BT_DIS_CONNECT [] = {
#include "res/cn/SOUND_DIS_CONNECT.txt"
};

const uint8_t CN_CHARGE_PLEASE[] = {
#include "res/cn/SOUND_CHARGE_PLEASE.txt"
};

const uint8_t CN_CHARGE_FINISH[] = {
};

const uint8_t CN_LANGUAGE_SWITCH[] = { 
};

const uint8_t CN_BT_WARNING[] = { 
#include "res/cn/SOUND_WARNING.txt"
};

const uint8_t CN_BT_ALEXA_START[] = { 

};

const uint8_t CN_BT_ALEXA_STOP[] = { 

};

const uint8_t CN_BT_GSOUND_MIC_OPEN[] = {

};

const uint8_t CN_BT_GSOUND_MIC_CLOSE[] = {

};

const uint8_t CN_BT_GSOUND_NC[] = {

};

const uint8_t CN_SOUND_CUSTOM_LEAK_DETECT [] = {
};

#ifdef __INTERACTION__
const uint8_t CN_BT_FINDME[] = {
};
#endif

/*doesn't have chinese version sound mute */
const uint8_t CN_BT_MUTE[] = {
#include "res/SOUND_MUTE.txt"
};

#endif


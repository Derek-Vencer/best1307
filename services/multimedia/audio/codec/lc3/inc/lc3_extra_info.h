#pragma once
#ifndef __LC3_EXTRA_INFO_H__
#define __LC3_EXTRA_INFO_H__
#include <stdint.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif

typedef struct LC3_Extra_Info
{
    void* tbl_list;
    void* math_list;

    uint16_t *I_fs;
    float *mdct_win;
}LC3_Extra_Info;

#endif


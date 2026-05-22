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
#ifdef BTH_IN_ROM
#include "bluetooth_bt_api.h"
#include "bt_callback_func.h"
#include "cqueue.h"
#include "multi_heap.h"
#include "multi_heap_internal.h"
#include "bth_rom_init.h"
#include "nvrecord_bt.h"
#include "app_utils.h"
#include "export_utils_fn_rom.h"
#include "export_sys_fn_rom.h"
#include "export_nv_fn_rom.h"
#include "cobuf.h"
#include "osif.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "plat_addr_map.h"
#include "os_porting.h"
#include "hal_location.h"

#include "multi_heap_porting.h"

// TODO: only support 1502x now
#define COHEAP_BIG_BUFFER_LOW_LIMIT (128)
#define COHEAP_BIG_BUFFER_HIGH_LIMIT (1024*3)

#define COHEAP_MAX_BUFFER_SIZE_FOR_SML (0)
#define COHEAP_MAX_BUFFER_SIZE_FOR_BIG (1024*30)

extern char __bth_ram_bin_start__[];
extern char __bth_ram_bin_end__[];

extern char __bth_data_start__[];
extern char __bth_data_end__[];
extern char __bth_bss_start__[];
extern char __bth_bss_end__[];

__attribute__((section(".patch_sram_bss"))) static uint8_t g_coheap_b_pool[COHEAP_MAX_BUFFER_SIZE_FOR_BIG] __attribute__((aligned(32)));

#if COHEAP_MAX_BUFFER_SIZE_FOR_SML
__attribute__((section(".patch_sram_bss"))) static uint8_t g_coheap_s_pool[COHEAP_MAX_BUFFER_SIZE_FOR_SML] __attribute__((aligned(32)));
#endif

struct coheap_global_t g_bt_rom_coheap_global;
#if !(defined(DEBUG) || defined(REL_TRACE_ENABLE))
extern int hal_trace_printf_internal(uint32_t attr, const char *fmt, va_list ap);
extern bool hal_trace_get_output_enable(void);
#endif
int bt_rom_printf(uint32_t attr, const char *fmt, va_list ap)
{
    int ret = 0;
#if !(defined(DEBUG) || defined(REL_TRACE_ENABLE))
    // in some test scenario,we need disable trace output
#ifdef CRASH_DUMP_ENABLE
    if (!hal_trace_in_crash_dump())
#endif
    {
        if (hal_trace_get_output_enable() == false)
            return 0;
    }

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }

    ret = hal_trace_printf_internal(attr, fmt, ap);

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }
#endif
    return ret;
}

struct coheap_global_t *bt_rom_export_coheap_global(void)
{
    return &g_bt_rom_coheap_global;
}

static void bth_stack_mem_init()
{
    struct coheap_global_t *g = &g_bt_rom_coheap_global;
    g->coheap_enable_debug = false;
    g->coheap_max_sml_size = COHEAP_MAX_BUFFER_SIZE_FOR_SML;
    g->coheap_max_big_size = COHEAP_MAX_BUFFER_SIZE_FOR_BIG;
    g->coheap_big_low_limit = COHEAP_BIG_BUFFER_LOW_LIMIT;
    g->coheap_big_high_limit = COHEAP_BIG_BUFFER_HIGH_LIMIT;
    g->coheap_b_pool = g_coheap_b_pool;
#if COHEAP_MAX_BUFFER_SIZE_FOR_SML
    g->coheap_s_pool = g_coheap_s_pool;
#else
    g->coheap_s_pool = NULL;
#endif
}

#if defined(CHIP_BEST1503)
static void bth_rom_load_bin()
{
    // Load data
    uint32_t *data_dst = (uint32_t*)__bth_data_start__;
    uint32_t *data_end = (uint32_t*)__bth_data_end__;
    // Saved in flash
    uint32_t *data_src = (uint32_t*)__bth_ram_bin_start__;
    uint32_t *data_src_end = (uint32_t*)__bth_ram_bin_end__;
    TRACE(2, "data section (%p,%p)->%p", data_src, data_src_end, data_dst);
    TRACE(2, "data start add = %p, end = %p", data_dst, data_end);
    for (; data_src < data_src_end; data_src++, data_dst++)
    {
        *data_dst = *data_src;
    }

    // clear BSS
    uint32_t *rom_bss_start = (uint32_t*)__bth_bss_start__;
    uint32_t *rom_bss_end = (uint32_t*)__bth_bss_end__;
    TRACE(2, "bss start addr = %p, end = %p", rom_bss_start, rom_bss_end );
    for (uint32_t* p= rom_bss_start ; p < rom_bss_end; p++)
    {
        *p = 0x00;
    }

    TRACE(1, "1503 %s end", __func__);
}
#else
static void bth_rom_load_bin()
{
    // Load data
    uint32_t *data_dst = (uint32_t*)__bth_data_start__;
    uint32_t *data_end = (uint32_t*)__bth_data_end__;
    // Saved in flash
    uint32_t *data_src = (uint32_t*)__bth_ram_bin_start__;
    uint32_t *data_src_end = (uint32_t*)__bth_ram_bin_end__;
    TRACE(2, "data section (%p,%p)->%p", data_src, data_src_end, data_dst);
    TRACE(2, "data start add = %p, end = %p", data_dst, data_end);
    for (; data_src < data_src_end; data_src++, data_dst++)
    {
        *data_dst = *data_src;
    }

    // clear BSS
    uint32_t *rom_bss_start = (uint32_t*)__bth_bss_start__;
    uint32_t *rom_bss_end = (uint32_t*)__bth_bss_end__;
    TRACE(2, "bss start addr = %p, end = %p", rom_bss_start, rom_bss_end );
    for (uint32_t* p= rom_bss_start ; p < rom_bss_end; p++)
    {
        *p = 0x00;
    }

    TRACE(1, "%s end", __func__);
}
#endif

static const struct EXPORT_NV_FN_ROM_T nv_fn = {
    .reserved = NULL,
    .nv_record_open = nv_record_open,
    .nv_record_add = nv_record_add,
    .nv_record_btdevicerecord_find = nv_record_btdevicerecord_find,
    .nv_record_flash_flush = nv_record_flash_flush,
    .nv_record_btdev_set_pnp_info = nv_record_btdevicerecord_set_pnp_info,
    .nv_record_ddbrec_find = nv_record_ddbrec_find,
    .nv_record_get_pnp_info = nv_record_get_pnp_info,
};

static const struct EXPORT_SYS_FN_ROM_T sys_fn = {
    .reserved = NULL,

    .hal_trace_dump = hal_trace_dump,
    .hal_trace_printf = bt_rom_printf,
    .hal_trace_output = hal_trace_output,

    .hal_sys_timer_get = hal_sys_timer_get,
    .hal_sys_timer_get_max = hal_sys_timer_get_max,

    .int_lock    = bt_int_lock,
    .int_unlock  = bt_int_unlock,

    .osThreadCreate = osThreadCreate,
    .osDelay= osDelay,

    .osMutexAcquire = osMutexAcquire,
    .osMutexCreate = osMutexCreate,
    .osMutexRelease = osMutexRelease,

    .osSemaphoreCreate = osSemaphoreCreate,
    .osTimerCreate = osTimerCreate,
    .osTimerStart = osTimerStart,
    .osTimerStop = osTimerStop,
    .osTimerDelete = osTimerDelete,

    .osMessageCreate = osMessageCreate,
    .osMessageGetSpace = osMessageGetSpace,
    .osMessageGet = osMessageGet,
    .osMessagePut = osMessagePut,

    .osSignalSet = osSignalSet,
    .osSignalWait = osSignalWait,

    .osif_init = osif_init,
    .osif_stop_hardware= osif_stop_hardware,
    .osif_resume_hardware = osif_stop_hardware,
    .osif_lock_stack = osif_lock_stack,
    .osif_unlock_stack = osif_unlock_stack,
    .osif_notify_evm = osif_notify_evm,
    .osif_lock_is_exist = osif_lock_is_exist,

    .bt_drv_digital_config_for_ble_adv = bt_drv_digital_config_for_ble_adv,
    .btdrv_load_sleep_config = btdrv_load_sleep_config,

    .app_sysfreq_req = app_sysfreq_req,
};

static const struct EXPORT_UTILS_FN_ROM_T utils_fn = {
    .reserved = NULL,

    .InitCQueue = InitCQueue,
    .DeCQueue = DeCQueue,
    .AvailableOfCQueue = AvailableOfCQueue,
    .EnCQueue = EnCQueue,
    .LengthOfCQueue = LengthOfCQueue,

    .multi_heap_register = multi_heap_register,
    .multi_heap_get_block_address = multi_heap_get_block_address,
    .multi_heap_malloc = multi_heap_malloc,
    .multi_heap_free = multi_heap_free,
    .multi_heap_get_info = multi_heap_get_info,
    .multi_heap_check_size_malloc_available = multi_heap_check_size_malloc_available,
    .multi_heap_lock = bt_multi_heap_lock,
    .multi_heap_unlock = bt_multi_heap_unlock,
};

void bth_rom_init(void)
{
    bth_rom_load_bin();

    bth_stack_mem_init();
    export_register_sys_fn(&sys_fn);
    export_register_utils_fn(&utils_fn);
    export_register_nv_fn(&nv_fn);

    TRACE(1, "%s ok", __func__ );
}

#endif /* BTH_IN_ROM */

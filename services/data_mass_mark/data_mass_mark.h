#ifndef __DATA_MASS_MARK_H__
#define __DATA_MASS_MARK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_timer.h"
#define IGNORE_HAL_TIMER_RAW_API_CHECK
#include "hal_timer_raw.h"

#define DATA_MASS_TAG0  (0)
#define DATA_MASS_TAG1  (1)
#define DATA_MASS_TAG2  (2)
#define DATA_MASS_TAG3  (3)
#define DATA_MASS_TAG4  (4)
#define DATA_MASS_TAG5  (5)
#define DATA_MASS_TAG6  (6)
#define DATA_MASS_TAG7  (7)
#define DATA_MASS_TAG8  (8)
#define DATA_MASS_TAG9  (9)
#define DATA_MASS_TAG10 (10)
#define DATA_MASS_TAG11 (11)
#define DATA_MASS_TAG12 (12)
#define DATA_MASS_TAG13 (13)
#define DATA_MASS_TAG14 (14)
#define DATA_MASS_TAG15 (15)
#define DATA_MASS_TAG16 (16)
#define DATA_MASS_TAG17 (17)
#define DATA_MASS_TAG18 (18)
#define DATA_MASS_TAG19 (19)
#define DATA_MASS_TAG20 (20)
#define DATA_MASS_TAG21 (21)
#define DATA_MASS_TAG22 (22)
#define DATA_MASS_TAG23 (23)
#define DATA_MASS_TAG24 (24)
#define DATA_MASS_TAG25 (25)
#define DATA_MASS_TAG26 (26)
#define DATA_MASS_TAG27 (27)
#define DATA_MASS_TAG28 (28)
#define DATA_MASS_TAG29 (29)
#define DATA_MASS_TAG30 (30)

#define DATA_MASS_MAGIC_NUMBER                   0xBE57EC1C

typedef struct
{
    uint32_t tag;
    uint32_t line;
    uint32_t elapsed_time;
    uint32_t in_timestamp;
    uint32_t out_timestamp;
    uint32_t wakeup_timestamp;
    uint32_t wakeup_src_map[(USER_IRQn_QTY + 31) / 32];
} DATA_MASS_SLEEP;

typedef struct
{
    uint32_t tag;
    uint32_t line;
} DATA_MASS_HAL;


#define DATA_MASS_ISPI_HISTORY_NUM (20)
typedef struct
{
    unsigned short reg;
    unsigned short val;
    uint32_t in_timestamp;
    uint32_t out_timestamp;
} DATA_MASS_ISPI;

typedef struct
{
    DATA_MASS_ISPI entry[DATA_MASS_ISPI_HISTORY_NUM];
    uint8_t index;
} DATA_MASS_ISPI_HISTORY;

#define DATA_MASS_IRQ_HISTORY_NUM (10)
typedef struct
{
    uint32_t irqn;
    uint32_t in_timestamp;
    uint32_t out_timestamp;
} DATA_MASS_IRQ;

typedef struct
{
    DATA_MASS_IRQ irq[DATA_MASS_IRQ_HISTORY_NUM];
    uint8_t index;
    uint32_t total_cnt;
} DATA_MASS_IRQ_HISTORY;

typedef struct
{
    uint32_t tag;
} DATA_MASS_FLASH;

typedef struct
{
    uint32_t task_id;  ///< Object Identifier
    uint8_t state;  ///< Object State
    uint32_t timestamp;
} DATA_MASS_OS;

typedef struct
{
    uint32_t tag;
} DATA_MASS_BT_PROFILE;

typedef struct
{
    uint32_t old_left_time;
    uint32_t new_left_time;
    uint32_t old_left_time_ms;
    uint32_t new_left_time_ms;
    uint32_t timestamp;
} DATA_MASS_WDT;

typedef struct
{
    uint32_t tag;
} DATA_MASS_CODEC;

typedef struct
{
    uint32_t tag;
} DATA_MASS_ANC;

typedef struct
{
    uint32_t magic;
#ifdef DATA_MASS_MARK_HAL
    DATA_MASS_HAL hal;
#endif
#ifdef DATA_MASS_MARK_IRQ
    DATA_MASS_IRQ_HISTORY irq_history;
#endif
#ifdef DATA_MASS_MARK_ISPI
    DATA_MASS_ISPI_HISTORY ispi_read_history;
    DATA_MASS_ISPI_HISTORY ispi_write_history;
#endif
#ifdef DATA_MASS_MARK_SLEEP
    DATA_MASS_SLEEP sleep;
#endif
#ifdef DATA_MASS_MARK_FLASH
    DATA_MASS_FLASH flash;
#endif
#ifdef DATA_MASS_MARK_OS
    DATA_MASS_OS os;
#endif
#ifdef DATA_MASS_MARK_WDT
    DATA_MASS_WDT wdt;
#endif
#ifdef DATA_MASS_MARK_BT_PROFILE
    DATA_MASS_BT_PROFILE bth;
#endif
#ifdef DATA_MASS_MARK_CODEC
    DATA_MASS_CODEC codec;
#endif
#ifdef DATA_MASS_MARK_ANC
    DATA_MASS_ANC anc;
#endif
} DATA_MASS_MARK_T;

extern DATA_MASS_MARK_T data_mass_mark;
extern uint32_t data_mass_run_magic;

#ifdef DATA_MASS_MARK_ENABLE
void __STATIC_FORCEINLINE data_mass_mark_hal_set_val(uint32_t val)
{
#ifdef DATA_MASS_MARK_HAL
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.hal.tag = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_hal_set_tag(uint32_t shift)
{
#ifdef DATA_MASS_MARK_HAL
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.hal.tag |= (1 << shift);
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_hal_set_line(uint32_t val)
{
#ifdef DATA_MASS_MARK_HAL
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.hal.line = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_set_val(uint32_t val)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.tag = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_tag(uint32_t shift)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.tag |= (1 << shift);
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_set_line(uint32_t val)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.line = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_entry(void)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.in_timestamp = hal_sys_timer_get();
    data_mass_mark.sleep.elapsed_time = hal_timer_get_elapsed_time();
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_exit(void)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.out_timestamp = hal_sys_timer_get();
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_sleep_save_wakeup_src(void)
{
#ifdef DATA_MASS_MARK_SLEEP
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.sleep.wakeup_timestamp = hal_sys_timer_get();
    for (uint32_t i = 0; i < ARRAY_SIZE(data_mass_mark.sleep.wakeup_src_map); i++) {
        data_mass_mark.sleep.wakeup_src_map[i] = (NVIC->ICPR[i] & NVIC->ISER[i]);
    }
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_os_set_val(uint32_t task_id, uint8_t state)
{
#ifdef DATA_MASS_MARK_OS
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.os.task_id = task_id;
    data_mass_mark.os.state = state;
    data_mass_mark.os.timestamp = hal_sys_timer_get();
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_wdt_set_val(uint32_t old_left_time, uint32_t new_left_time, uint32_t old_left_time_ms, uint32_t new_left_time_ms)
{
#ifdef DATA_MASS_MARK_WDT
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.wdt.timestamp = hal_sys_timer_get();
    data_mass_mark.wdt.old_left_time = old_left_time;
    data_mass_mark.wdt.new_left_time = new_left_time;
    data_mass_mark.wdt.old_left_time_ms = old_left_time_ms;
    data_mass_mark.wdt.new_left_time_ms = new_left_time_ms;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_flash_set_val(uint32_t val)
{
#ifdef DATA_MASS_MARK_FLASH
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.flash.tag = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_flash_tag(uint32_t shift)
{
#ifdef DATA_MASS_MARK_FLASH
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.flash.tag |= (1 << shift);
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_bth_set_val(uint32_t val)
{
#ifdef DATA_MASS_MARK_BT_PROFILE
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.bth.tag = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_mark_bth_tag(uint32_t shift)
{
#ifdef DATA_MASS_MARK_BT_PROFILE
    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    data_mass_mark.bth.tag |= (1 << shift);
#endif
}

void __STATIC_FORCEINLINE data_mass_reg_read_entry(unsigned short reg, unsigned short *val)
{
#ifdef DATA_MASS_MARK_ISPI
    DATA_MASS_ISPI *spi_entry;

    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    spi_entry = &data_mass_mark.ispi_read_history.entry[data_mass_mark.ispi_read_history.index];
    spi_entry->in_timestamp = hal_sys_timer_get();
    spi_entry->out_timestamp = 0xffffffff;

    spi_entry->reg = reg;
    spi_entry->val = 0xcccc;
#endif
}


void __STATIC_FORCEINLINE data_mass_reg_read_exit(unsigned short reg, unsigned short *val)
{
#ifdef DATA_MASS_MARK_ISPI
    DATA_MASS_ISPI *spi_entry;

    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    spi_entry = &data_mass_mark.ispi_read_history.entry[data_mass_mark.ispi_read_history.index];

    spi_entry->out_timestamp = hal_sys_timer_get();
    if (spi_entry->reg == reg){
        spi_entry->val = *val;
    }else{
        spi_entry->val = 0xdead;
    }

    data_mass_mark.ispi_read_history.index = (data_mass_mark.ispi_read_history.index + 1) % DATA_MASS_ISPI_HISTORY_NUM;
#endif
}

void __STATIC_FORCEINLINE data_mass_reg_write_entry(unsigned short reg, unsigned short val)
{
#ifdef DATA_MASS_MARK_ISPI
    DATA_MASS_ISPI *spi_entry;

    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    spi_entry = &data_mass_mark.ispi_write_history.entry[data_mass_mark.ispi_write_history.index];

    spi_entry->in_timestamp = hal_sys_timer_get();
    spi_entry->out_timestamp = 0xffffffff;

    spi_entry->reg = reg;
    spi_entry->val = val;
#endif
}

void __STATIC_FORCEINLINE data_mass_reg_write_exit(unsigned short reg, unsigned short val)
{
#ifdef DATA_MASS_MARK_ISPI
    DATA_MASS_ISPI *spi_entry;

    if (data_mass_run_magic != DATA_MASS_MAGIC_NUMBER){
        return;
    }

    spi_entry = &data_mass_mark.ispi_write_history.entry[data_mass_mark.ispi_write_history.index];

    if (spi_entry->reg == reg){
        spi_entry->out_timestamp = hal_sys_timer_get();
    }
    data_mass_mark.ispi_write_history.index = (data_mass_mark.ispi_write_history.index + 1) % DATA_MASS_ISPI_HISTORY_NUM;
#endif
}
#else
void __STATIC_FORCEINLINE data_mass_mark_hal_set_val(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_hal_set_tag(uint32_t shift){}
void __STATIC_FORCEINLINE data_mass_mark_hal_set_line(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_set_val(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_tag(uint32_t shift){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_set_line(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_entry(void){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_exit(void){}
void __STATIC_FORCEINLINE data_mass_mark_sleep_save_wakeup_src(void){}
void __STATIC_FORCEINLINE data_mass_mark_os_set_val(uint32_t task_id, uint8_t state){}
void __STATIC_FORCEINLINE data_mass_mark_wdt_set_val(uint32_t old_left_time, uint32_t new_left_time, uint32_t old_left_time_ms, uint32_t new_left_time_ms){}
void __STATIC_FORCEINLINE data_mass_mark_flash_set_val(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_flash_tag(uint32_t shift){}
void __STATIC_FORCEINLINE data_mass_mark_bth_set_val(uint32_t val){}
void __STATIC_FORCEINLINE data_mass_mark_bth_tag(uint32_t shift){}
void __STATIC_FORCEINLINE data_mass_reg_read_entry(unsigned short reg, unsigned short *val){}
void __STATIC_FORCEINLINE data_mass_reg_read_exit(unsigned short reg, unsigned short *val){}
void __STATIC_FORCEINLINE data_mass_reg_write_entry(unsigned short reg, unsigned short val){}
void __STATIC_FORCEINLINE data_mass_reg_write_exit(unsigned short reg, unsigned short val){}
#endif

int data_mass_mark_dump(void);
int data_mass_mark_save(void);
int data_mass_mark_open(void);
int data_mass_mark_close(void);

#ifdef __cplusplus
}
#endif

#endif


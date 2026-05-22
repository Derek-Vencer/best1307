
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
ifeq ($(USE_LIB),1)
mmlib_obj += cmsis/
ifeq ($(PROGRAMMER),)
mmlib_obj += drivers/
ifeq ($(BECO),1)
mmlib_obj += beco/
endif
else
mmlib_obj += drivers/norflash/
mmlib_obj += drivers/ana/
endif
endif
PLATFORM_LIB_NAME := libplatform
ifeq ($(ANC_APP), 1)
PLATFORM_LIB_NAME := $(PLATFORM_LIB_NAME)_anc
endif
ifeq ($(USE_TRACE_ID), 1)
PLATFORM_LIB_NAME := $(PLATFORM_LIB_NAME)_log_compressed
endif
ifeq ($(PROGRAMMER),1)
PLATFORM_LIB_NAME := $(PLATFORM_LIB_NAME)_progarmmer
endif
$(PLATFORM_LIB_NAME)-y := $(mmlib_obj)

obj-y += $(PLATFORM_LIB_NAME).a

subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal -Iutils/hwtimer_list
subdir-ccflags-y += -Iinclude/cmsis_nn -Iinclude/cmsis_dsp
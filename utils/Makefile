cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

utilslib_obj :=
ifeq ($(USE_LIB),1)
utilslib_obj += \
    heap/ \
    hwtimer_list/ \
    lzma/ \
    crc32/ \
    libc/
ifeq ($(PROGRAMMER),)
utilslib_obj += \
    codec_calib/ \
    crash_catcher/ \
    kfifo/ \
    retention_ram/ \
    signal_generator/ \
    xyzmodem/ \
    crc16/ \
    sha256/ \
    cqueue/ \
    list/
ifeq ($(USE_UTILS_HEXDUMP),1)
utilslib_obj += hexdump/
endif
ifeq ($(CTKD_ENABLE),1)
utilslib_obj += aes_cmac/
endif
else
utilslib_obj += hexdump/
utilslib_obj += bsdiff/
endif

endif
UTILS_LIB_NAME := libutils
ifeq ($(USE_TRACE_ID),1)
UTILS_LIB_NAME := $(UTILS_LIB_NAME)_log_compressed
endif
ifeq ($(PROGRAMMER),1)
UTILS_LIB_NAME := $(UTILS_LIB_NAME)_progarmmer
endif
ifeq ($(OTA_USE_DIFF),1)
UTILS_LIB_NAME := $(UTILS_LIB_NAME)_diff
endif
$(UTILS_LIB_NAME)-y := $(utilslib_obj)

obj-y += $(UTILS_LIB_NAME).a




cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)test/*.cpp))

ifeq ($(BTH_SUBSYS_ONLY), 1)
obj_c_filterout := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*_ipc_api.c))
obj_cpp_filterout := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*_ipc_api.cpp))

obj_c := $(filter-out $(obj_c_filterout), $(obj_c))
obj_cpp := $(filter-out $(obj_cpp_filterout), $(obj_cpp))
endif

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

AM_LIB_NAME := libaudio_manager

ifeq ($(BLE_AUDIO_ENABLED), 1)
AM_LIB_NAME := $(AM_LIB_NAME)_aob
endif

ifeq ($(USE_TRACE_ID), 1)
AM_LIB_NAME := $(AM_LIB_NAME)_log_compressed
endif

$(AM_LIB_NAME)-y := $(src_obj)

obj-y :=  $(AM_LIB_NAME).a

CFLAGS_app_audio_active_device_manager.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += \
    -I$(BT_ROM_PORTING_PATH) \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -I$(BLE_GAF_CORE_DIR_PATH)/inc \
    -I$(BLE_AOB_APP_DIR_PATH)/inc \
    -Iservices/bt_app \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/multimedia/audio/process/resample/include \
    -Iapps/nv_section/factory_section \
    -Iapps/nv_section/fpga_section \
    -Iservices/lea_player/inc \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/audioflinger \
    -Iapps/audio_dump/include \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iutils/hsm \
    -Iapps/key \
    -Iapps/common_ui \
    -Iapps/main \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/anc/inc \
    -Iapps/factory \
    -Iservices/interconnection/green \
    -Iutils/hwtimer_list \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iapps/nv_section/log_section \
    -Iservices/ai_voice/manager \
    -Iservices/app_ai/inc \
    -Iapps/battery/ \
    -Iutils/crc16 \
    -Iutils/crc32 \
    -Iutils/rom_utils \
    -Iservices/multimedia/audio/codec/lc3/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/ux/common/ibrt/ibrt_middleware/inc  \
    -Iservices/ota \
    -Iservices/audioflinger \
    -Iservices/ux/common/ble_audio_core/inc \
    -Iutils/list \
    -Iapps/audio_manager \
    -Iservices/audio_manager/test

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
subdir-ccflags-y += \
    -Iservices/ux/common/ibrt/app_ibrt/inc \
    -Iservices/ux/common/ibrt/ibrt_ui/inc \
    -Iservices/ux/common/ibrt/ibrt_core/inc 
endif
endif

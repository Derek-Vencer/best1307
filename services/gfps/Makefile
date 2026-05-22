
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_ble.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_rfcomm.cpp))

obj_lib_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_crypto.c))
obj_lib_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/gfps_sass.cpp))
#CRYPTO_LIB_NAME := libgfps_crypto
#$(CRYPTO_LIB_NAME)-y := $(obj_c:.c=.o)

libgfps_crypto-y := $(obj_lib_c:.c=.o) $(obj_lib_cpp:.cpp=.o)
obj-y := libgfps_crypto.a
obj-y += $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -I$(BLE_STACK_INCLUDES) \
	-I$(BLE_APP_INCLUDES) \
    -I$(BLUETOOTH_ADAPTER_INCLUDES) \
    -I$(BT_APP_DIR_PATH)/inc \
    -I$(BT_APP_DIR_PATH)/inc/a2dp_codecs \
    -I$(BT_IF_DIR_PATH)/inc \
	-I$(BT_PROFILES_DIR_PATH)/inc \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/adapter_service \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/common \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/me \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/ibrt \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/l2cap \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/a2dp \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/avrcp \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/hfp \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/dip \
	-I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/bt/source \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/common \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gap \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gatt \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/aob \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/gfps \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/dp \
    -I$(BLUETOOTH_ADAPTER_DIR_PATH)/inc/ble/ai \
    -Iservices/gfps/inc \
    -Iutils/cqueue \
    -Iutils/encrypt \
    -Imultimedia/inc/audio/codec/sbc/inc \
    -Iservices/ux/common/ibrt/ibrt_core/inc \
    -Iservices/ux/common/ibrt/app_ibrt/inc \
    -Iservices/ux/common/ibrt/ibrt_middleware/inc \
    -Iservices/ux/common/ibrt/ibrt_ui/inc \
    -Iplatform/drivers/bt \
    -Iapps/key \
    -Iapps/common_ui \
    -Iapps/main \
	-Iapps/earbuds/conn \
    -Iservices/audio_manager \
    -Iservices/audio_bt \
	-Iservices/ux/product/earbud_tws/inc \
	-Iutils/hwtimer_list \
	-Iapps/nv_section/customparam_section \
	-Iutils/hsm \
	-Iservices/ux/common/ble_audio_core/inc



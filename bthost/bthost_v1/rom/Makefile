cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))


ifeq ($(CHIP),best1503)
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1503_t2/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1503_t2/patch/*.c))
else ifeq ($(CHIP),best1502x)
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1502x_simu/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1502x_simu/patch/*.c))
else ifeq ($(CHIP),best1307)
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1307_simu/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1307_simu/patch/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)1307_simu/patch/*.cpp))
else
$(error This chip not support bth-rom)
endif

BTHROM_PATCH_LIB_NAME := bth_rom_patch

$(BTHROM_PATCH_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(BTHROM_PATCH_LIB_NAME).a

ifeq ($(CHIP),best1503)
ccflags-y += -I$(BLUETOOTH_BTH_DIR_PATH)/rom/1503_t2/patch
else ifeq ($(CHIP),best1502x)
ccflags-y += -I$(BLUETOOTH_BTH_DIR_PATH)/rom/1502x_simu/patch
else ifeq ($(CHIP),best1307)
ccflags-y += -I$(BLUETOOTH_BTH_DIR_PATH)/rom/1307_simu/patch
else
$(error This chip not support bth-rom)
endif


ccflags-y += \
    -Iservices/audio_bt \
	$(AAC_INCLUDES) \
	-Iinclude/cmsis_dsp \
	-Iservices/voice_dev \
	-Iservices/audio_process \
	-Iapps/audio_dump/include \
	-Iservices/hw_dsp/inc \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	$(BES_BT_IMPL_INCLUDES) \
	$(BLE_APP_INCLUDES) \
	$(BLE_STACK_INCLUDES) \
	-Imultimedia/inc/speech/inc \
	-Iservices/bone_sensor \
	-Iservices/overlay \
	-Iservices/audio_manager \
	-Ithirdparty/tile/tile_common/tile_storage \
	-Ithirdparty/tile/tile_common/tile_features \
	-Iservices/resources \
	-Iservices/multimedia/audio/process/include \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/anc/include\
	-Iservices/multimedia/audio/process/sidetone/include\
	-Iservices/multimedia/audio/process/adj_mc/inc\
	-Iapps/nv_section/aud_section \
	-Iapps/nv_section/userdata_section \
	-Iapps/nv_section/include \
	-Iservices/voicepath/$(VOICE_DATAPATH_TYPE) \
	-Iservices/voicepath/gsound/gsound_target \
	-Iservices/voicepath/gsound/gsound_custom/inc \
	-Iservices/voicepath/gsound/gsound_target_api_read_only \
	-Iplatform/drivers/uarthci \
	-Iplatform/drivers/ana \
	-Iplatform/cmsis \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iutils/heap \
	-Iutils/crc16 \
	-Iutils/rom_utils \
	-Iservices/audioflinger \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/anc/inc \
	-Iapps/key \
	-Iapps/common_ui \
    -Iapps/main \
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/audioplayers/a2dp_decoder \
	-Iapps/battery \
	-Iapps/common \
	-Iapps/factory \
	-Iapps/ble_audio_test \
	-Iapps/voice_assist/inc \
	-Iutils/hwtimer_list \
	-Iservices/voicepath \
	-Ithirdparty/userapi \
	-Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
	-Iservices/ai_voice/manager \
	-Iservices/ai_voice/audio \
	-Iservices/ai_voice/transport \
	-Iservices/app_ai/inc \
	-Iservices/interconnection/red \
	-Iservices/interconnection/green \
	-Iservices/interconnection/umm_malloc \
	-Iservices/ux/common/ibrt/custom_api \
	-Iservices/ux/common/ble_audio_core/inc \
	$(SOURCE_INCLUDES) \
	-Ithirdparty/tile/tile_target \
	-Iservices/osif \
	-Iservices/ux/common/ibrt/ibrt_middleware/inc \
	-Iutils/list \
	-Iutils/string \
	-Iservices/ai_voice/protocol/bixbyvoice \
	-Iservices/ai_voice/protocol/bixbyvoice/bixbyvoice_manager \
	-Iservices/bt_service/inc \
	-Iservices/bt_sync \
    -Iservices/lea_player/gaf_audio \
	-Iservices/lea_player/inc \
	-Iutils/rom_utils/$(CHIP) \
	-I$(BT_ROM_PATCH_PATH) \
    -I$(BT_ROM_PORTING_PATH)
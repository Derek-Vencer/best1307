cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(A2DP_AAC_ON),1)
AAC_INCLUDES = -Iservices/multimedia/audio/codec/aac/inc
else
AAC_INCLUDES =
endif

ifeq ($(AUDIO_TRIGGER_VER),1)
ccflags-y +=   \
		-Iapps/audioplayers/audio_trigger/version1
endif

ccflags-y += \
	$(AAC_INCLUDES) \
	-Iinclude/cmsis_dsp \
	-Iservices/voice_dev \
	-Iservices/audio_process \
	-Iapps/audio_dump/include \
	-Iservices/hw_dsp/inc \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	-Iservices/bone_sensor \
	-Iservices/overlay \
	-Ithirdparty/tile/tile_common/tile_storage \
	-Iservices/resources \
	-Iutils/kfifo \
	-Iservices/speech_algo_dsp \
	-Imultimedia/inc/speech/inc \
	-Imultimedia/inc/rbcodec \
	-Imultimedia/inc/audio/process/resample/include \
	-Imultimedia/inc/audio/process/filters/include \
	-Imultimedia/inc/audio/process/drc/include \
	-Imultimedia/inc/audio/process/anc/include\
	-Imultimedia/inc/audio/process/sidetone/include\
	-Imultimedia/inc/audio/process/adj_mc/inc\
	-Iservices/multimedia/speech/inc \
	-Iservices/multimedia/rbcodec \
	-Iservices/multimedia/audio/process/resample/include \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/anc/include\
	-Iservices/multimedia/audio/process/sidetone/include\
	-Iservices/multimedia/audio/process/adj_mc/inc\
	-Iservices/multimedia/audio/process/integer_resampling/include \
	-Iapps/nv_section/aud_section \
	-Iapps/nv_section/userdata_section \
	-Iapps/nv_section/include \
	-Iservices/voicepath/$(VOICE_DATAPATH_TYPE) \
	-Iservices/voicepath/gsound/gsound_target \
	-Iservices/voicepath/gsound/gsound_custom/inc \
	-Iservices/voicepath/gsound/gsound_target_api_read_only \
	-Iservices/gfps/inc \
	-Iplatform/drivers/uarthci \
	-Iplatform/drivers/ana \
	-Iplatform/cmsis \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iutils/heap \
	-Iutils/crc16 \
	-Iservices/audioflinger \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/anc/inc \
	-Iapps/key \
	-Iapps/karaoke \
	-Iapps/common_ui \
    -Iapps/main \
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/audioplayers/a2dp_decoder \
	-Iapps/battery \
	-Iapps/common \
	-Iapps/factory \
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
	$(SOURCE_INCLUDES) \
	-Ithirdparty/tile/tile_target \
	-Iservices/osif \
	-Iservices/ux/common/ibrt/ibrt_middleware/inc  \
    -Iutils/list \
    -Iutils/string \
    -Iutils/cqueue \
    -Iservices/ai_voice/protocol/bixbyvoice \
    -Iservices/ai_voice/protocol/bixbyvoice/bixbyvoice_manager \
	-Iservices/bt_service/inc \
    -Iservices/audio_manager\
	-Iplatform/drivers/ana/best1307

ccflags-y += -Iservices/multimedia/audio/codec/sbc/inc

ifeq ($(GFPS_ENABLE),1)
ccflags-y += \
   -Iservices/ble_app/app_gfps \
   -Iservices/gfps/inc
endif

ifeq ($(ANC_NOISE_TRACKER),1)
ccflags-y += \
	-Ithirdparty/noise_tracker_lib \
	-Ithirdparty/userapi/noise_tracker_app
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
ccflags-y += -Iapps/btusbaudio
endif

ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
ccflags-y += -Iapps/btusbaudio
ccflags-y += $(BLUETOOTH_ADAPTER_INCLUDES)
endif

ifeq ($(INTERCONNECTION),1)
ccflags-y += $(BLUETOOTH_ADAPTER_INCLUDES)
endif

ifeq ($(INTERACTION),1)
ccflags-y += $(BLUETOOTH_ADAPTER_INCLUDES)
endif

ifeq ($(A2DP_LDAC_ON),1)
ccflags-y += -Ithirdparty/audio_codec_lib/ldac/inc
endif

ifeq ($(RB_CODEC),1)
ccflags-y += -DRB_CODEC
endif

ifeq ($(TEST_OVER_THE_AIR),1)
ccflags-y += -Iservices/tota
endif

ifeq ($(TEST_OVER_THE_AIR_v2),1)
ccflags-y += -Iapps/tota_v2
endif

ccflags-y += \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(EPLAYER_INCLUDES) \
    $(ECOMM_INCLUDES) \
	-Iapps/apptester \
	-Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
	-Iapps/anc/inc \
	-Iapps/voice_assist/inc \
	-Iapps/common \
	-Iapps/sdmmc \
	-Iapps/sdmmc_msd \
	-Iapps/common_ui \
    -Iapps/main \
	-Iapps/cmd \
	-Iapps/key \
	-Iapps/pwl \
	-Iapps/battery \
	-Iservices/voicepath \
	-Iapps/audio_dump/include \
	-Iutils/list \
	-Iutils/heap \
	-Iservices/audio_manager \
	-Iservices/multimedia/audio/process/filters/include \
    -Iservices/ota/bes_ota/inc \
	-Iapps/key\
	-Iservices/ux/common/ibrt/ibrt_core/inc \
    -Iapps/tota_v2 \
    -Iinclude/watchdog \
    -Iapps/battery\
    -Iplatform/drivers/ana/best1307\
    -Iapps/audioplayers/audio_trigger/version1\
    -Iplatform/hal/best1307\
    -Ibthost/bthost_v1/service/ble_app_new/inc\
    -Iutils/lockcqueue\
    -Iservices/multimedia/audio/codec/sbc/inc\
    -Iservices/multimedia/audio/process/drc/include\
    -Iservices/multimedia/audio/process/limiter/include\
    -Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/common/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/limiter/include \
	-Iservices/multimedia/audio/process/reverb/include \
	-Iservices/multimedia/audio/process/bass_enhancer/include \
	-Iservices/multimedia/audio/process/virtual_surround_process/include \
	-Iservices/multimedia/speech/inc \
	-Iplatform/drivers/bt \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iutils/cqueue \
	-Iutils/crc32 \
	-Iutils/heap \
	-Iapps/anc/inc \
	-Iapps/key \
	-Iapps/audioplayers \
	-Iservices/bt_app \
	-Iservices/ux/common/ibrt/ibrt_middleware/inc  \
	$(BLUETOOTH_ADAPTER_INCLUDES) \
	-Iapps/nv_section/aud_section \
	-Iapps/nv_section/include \
	-Iservices/hw_dsp/inc \
	-Iapps/audio_dump/include \
	-Iservices/audioflinger \
	-Iservices/audio_manager \
	-Iinclude/cmsis_dsp \
	-Iservices/norflash_api \
	-Iutils/heap \
	-Iapps/common/ \
	-Iapps/audioplayers/a2dp_decoder/ \
	-Iservices/audio_bt\
    -Iservices/multimedia/audio/process/resample/include\
    -Iservices/voice_dev
ccflags-y +=  \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(EPLAYER_INCLUDES) \
    $(ECOMM_INCLUDES) \
    -Iservices/audio_bt \
    -Iservices/audioflinger \
    -Iservices/norflash_api \
    -Iapps/nv_section/factory_section \
    -Iapps/nv_section/log_section \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_process \
    -Iutils/intersyshci \
    -Iapps/apptester \
    -Iapps/factory \
    -Iapps/voice_detector \
    -Iutils/crc32 \
    -Iutils/list \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iapps/audioplayers/rbplay \
    -Itests/anc_usb \
    -Iapps/anc/inc \
    -Iapps/btspeaker \
    -Ithirdparty/userapi \
    -Iapps/communication \
    -Iutils/cqueue \
    -Iservices/ai_voice/audio \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iservices/interconnection/red \
    -Iservices/interconnection/green \
    -Iservices/bridge/ \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/aob_app/inc \
    -Iservices/aob_bes_ux/inc \
    -Iservices/aob_custom_ux \
    -Ithirdparty/tile \
    -Iservices/ux/common/ibrt/ibrt_middleware/inc  \
    -Iservices/rpc/inc \
    -Ithirdparty/tile/tile_target \
    -Iinclude/ \
    -Iapps/btwatch \
    -Iapps/sensorhub \
    -Iapps/dsp_m55 \
    -Iplatform/drivers/cp_accel \
    -Iservices/audio_manager \
    -Iplatform/main/cmse \
	-Iplatform/drivers/ana/best1307 \
	-Iservices/gfps/inc \
	-Iapps/key \
	-Iapps/common\
    -Iapps/capsensor\
    -Iplatform/drivers/capsensor/best1307


ifeq ($(IBRT),1)
ifeq ($(IBRT_UI),1)
ccflags-y += \
    -Iservices/ux/common/ibrt/app_ibrt/inc \
    -Iservices/ux/common/ibrt/ibrt_core/inc  \
    -Iservices/ux/common/ibrt/custom_api \
    -Iservices/ux/common/ibrt/ibrt_ui/inc \
    -Iservices/ux/product/earbud_tws/inc \
    -Iapps/earbuds/conn
endif
endif

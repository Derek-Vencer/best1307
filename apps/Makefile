obj-y := audioplayers/ common/ main/ key/ pwl/ battery/ factory/ cmd/ mic/ audio_test/ bt_sync/ communication/ nv_section/ audio_dump/

ifeq ($(CAPSENSOR_SPP_SERVER),1)
obj-y += app_spp/
endif

ifeq ($(APP_TEST_AUDIO),1)
obj-y += apptester/
endif

obj-y += common_ui/


ifeq ($(TEST_OVER_THE_AIR_v2),1)
obj-y += tota_v2/
endif


ifeq ($(BLE_AUDIO_ENABLED),1)
obj-y += ble_audio_test/
endif

ifeq ($(APP_TEST_SDMMC),1)
obj-y += sdmmc/
endif

ifeq ($(MSD_MODE),1)
obj-y += sdmmc_msd/
endif

ifeq ($(ANC_APP),1)
obj-y += anc/
endif

ifeq ($(ANC_ASSIST_ENABLED),1)
obj-y += voice_assist/
endif

ifeq ($(VOICE_DETECTOR_EN),1)
obj-y += voice_detector/
endif

ifeq ($(SENSOR_HUB),1)
obj-y += sensorhub/
endif

ifeq ($(DSP_M55),1)
obj-y += dsp_m55/
endif

ifeq ($(AUDIO_HEARING_COMPSATN),1)
obj-y += hearing_detec/
endif

_APP_BT_WATCH ?= 0
ifeq ($(BT_WATCH_MASTER),1)
_APP_BT_WATCH := 1
endif
ifeq ($(BT_WATCH_SLAVE),1)
_APP_BT_WATCH := 1
endif
ifeq ($(_APP_BT_WATCH),1)
obj-y += btwatch/
endif

ifeq ($(APP_RPC_ENABLE),1)
obj-y += app_rpc/
endif

ifeq ($(APP_SOUND_ENABLE),1)
obj-y += sound/
endif

ifeq ($(SPEECH_ALGO_DSP),1)
obj-y += speech_algo_dsp/
endif

ifeq ($(APP_BT_SPEAKER),1)
obj-y += btspeaker/
endif

ifeq ($(BLE_WALKIE_TALKIE),1)
obj-y += ble_walkie_talkie/
endif

# include earbud in default
ifneq ($(BT_WATCH_APP),1)
obj-y += earbuds/
endif

export APP_SPP_DEMO ?= 0
ifeq ($(APP_SPP_DEMO),1)
obj-y += example/app_spp/
endif

ifeq ($(HEAD_TRACK_ENABLE),1)
obj-y += head_track_3dof/
endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
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
					$(BLUETOOTH_ADAPTER_INCLUDES) \
					-Iutils/list \
					-Iutils/heap \
					-Iservices/audio_manager \
					-Iservices/multimedia/audio/process/filters/include

ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
subdir-ccflags-y += -Iapps/btusbaudio
endif

ifeq ($(A2DP_LDAC_ON),1)
subdir-ccflags-y += -Ithirdparty/audio_codec_lib/ldac/inc
endif

ifeq ($(HEAD_TRACK_ENABLE),1)
subdir-ccflags-y += -Iservices/head_track
endif
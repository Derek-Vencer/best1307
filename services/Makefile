cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
serviceslib_obj :=
ifeq ($(USE_LIB),1)
serviceslib_obj := \
    audio_bt/ \
    audioflinger/ \
    audio_process/ \
    hw_dsp/ \
    voice_dev/ \
    overlay/ \
    osif/ \
    norflash_api/ \

ifneq ($(filter 1, $(GATT_RATE_TESTC) $(GATT_RATE_TESTS)),)
serviceslib_obj += rate_test_demo/
endif

ifeq ($(SPEECH_BONE_SENSOR),1)
serviceslib_obj += bone_sensor/
endif

ifneq ($(BT_BUILD_WITH_CUSTOMER_HOST),1)
ifneq ($(filter 1,$(INTERCONNECTION) $(INTERACTION) $(AI_VOICE)),)
serviceslib_obj += interconnection/
endif
endif


ifeq ($(MBED),1)
serviceslib_obj += fs/
endif

ifeq ($(AUTO_TEST),1)
serviceslib_obj += auto_test/
endif


ifeq ($(RPC_SUPPORT),1)
serviceslib_obj += rpc/
endif

ifneq ($(BT_BUILD_WITH_CUSTOMER_HOST),1)
ifeq ($(VOICE_DATAPATH_ENABLED),1)
serviceslib_obj += voicepath/
serviceslib_obj += voicepath/$(VOICE_DATAPATH_TYPE)/
endif

ifneq ($(filter 1, $(VOICE_DATAPATH_ENABLED) $(AI_VOICE)),)
serviceslib_obj += app_ai/
endif

ifneq ($(IBRT_UI),1)
obj-y += ../utils/hsm/
endif

ifeq ($(VOICE_RECOGNITION),1)
serviceslib_obj += voice_recognition/
endif

ifneq ($(filter 1, $(BES_OTA) $(AI_OTA) $(GMA_VOICE)),)
ifeq ($(FPGA_IBRT_OTA),1)
serviceslib_obj += fpga_ibrt_ota/
else
serviceslib_obj += ota/
endif
endif

ifeq ($(TILE_DATAPATH_ENABLED),1)
obj-y += ../thirdparty/tile/
endif

ifeq ($(AI_VOICE),1)
serviceslib_obj += ai_voice/
endif

ifeq ($(GFPS_ENABLE),1)
serviceslib_obj += gfps/
endif

ifeq ($(DATA_MASS_MARK_ENABLE),1)
serviceslib_obj += data_mass_mark/
endif

ifeq ($(THROUGH_PUT),1)
serviceslib_obj += through_put/
endif

ifeq ($(EPLAYER_ON),1)
serviceslib_obj += eplayer/eplayer/
ifeq ($(EPLAYER_TEST),1)
serviceslib_obj += eplayer/tests/
endif
endif

ifeq ($(ETRANSPORT_ON),1)
obj-y += etransport/
endif

ifeq ($(ECOMM_ON),1)
serviceslib_obj += ecomm/
endif

ifeq ($(BT_SERVICE_ON),1)
ifeq ($(BT_SERVICE_NATIVE),1)
obj-y += bt_service/native/
endif
ifeq ($(BT_SERVICE_DISTRIBUTE),1)
obj-y += bt_service/distribute/
endif
ifeq ($(BT_SERVICE_TEST),1)
obj-y += bt_service/tests/
endif
endif

ifeq ($(EAUDIO_ON),1)
serviceslib_obj += eaudio/
serviceslib_obj += eaudio/device/
serviceslib_obj += eaudio/policy/
serviceslib_obj += eaudio/effect/
serviceslib_obj += eaudio/resample/
serviceslib_obj += eaudio/heap/
ifeq ($(EAUDIO_TEST),1)
serviceslib_obj += eaudio/tests/
endif
endif

ifeq ($(BLE_WALKIE_TALKIE),1)
serviceslib_obj += walkie_talkie/
endif
endif # BT_BUILD_WITH_CUSTOMER_HOST

ifeq ($(ACTIVITY_MANAGER_ENABLE),1)
serviceslib_obj += activity_manager/
endif

ifeq ($(CAPSENSOR_ENABLE),1)
serviceslib_obj += capsensor/
endif

ifeq ($(BLE),1)
ifeq ($(BLE_AUDIO_ENABLED),1)
serviceslib_obj += lea_player/
endif
endif

ifeq ($(HEAD_TRACK_ENABLE),1)
serviceslib_obj += head_track/
endif

ifeq ($(SPP_TEST_ENABLE),1)
serviceslib_obj += spp_test/
endif
endif
SERVICES_LIB_NAME := libservices

ifeq ($(AUDIO_SECTION_ENABLE), 1)
SERVICES_LIB_NAME := $(SERVICES_LIB_NAME)_eq
endif
ifeq ($(BLE), 1)
SERVICES_LIB_NAME := $(SERVICES_LIB_NAME)_ble
endif
ifeq ($(BLE_AUDIO_ENABLED), 1)
SERVICES_LIB_NAME := $(SERVICES_LIB_NAME)_aob
endif
ifeq ($(USE_TRACE_ID), 1)
SERVICES_LIB_NAME := $(SERVICES_LIB_NAME)_log_compressed
endif

$(SERVICES_LIB_NAME)-y := $(serviceslib_obj)

obj-y := $(SERVICES_LIB_NAME).a

obj-y += ux/

obj-y += audio_manager/

obj-y += capsensor/capsensor_algorithm/

ifeq ($(TEST_OVER_THE_AIR),1)
obj-y += tota/
endif



ifeq ($(APP_ANC_TEST),1)
obj-y += anc_spp_tool/
endif

ifeq ($(IBRT),1)
obj-y += ux/common/ibrt/app_ibrt/
obj-y += ux/common/config/
endif


obj-y :=
aac_api-y :=
ifeq ($(AAC_IN_ROM),1)
aac_api-y += aac_rom_adapter/
else ifeq ($(AAC_REDUCE_SIZE),1)
obj-y += fdkaac_codec_2_0_1_reduce/
else
obj-y += fdkaac_codec_2_0_1/
endif
aac_api-y += api/
obj-y += aac_api.o
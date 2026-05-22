obj-y :=

ifeq ($(ROM_UTILS_ON)-$(CHIP),1-best1503)
obj-y +=
else ifeq ($(ROM_UTILS_ON)-$(CHIP),1-best1307)
obj-y +=
else ifeq ($(ROM_UTILS_ON)-$(CHIP),1-best1306)
obj-y += sbc_rom_adapter/
else ifeq ($(ROM_UTILS_ON),1)
obj-y += sbc/src/
else
obj-y += sbc_reduce/
endif

sbc_api-y := sbc_api/
obj-y += sbc_api.o

subdir-ccflags-y += -O2

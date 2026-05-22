cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := bsdiff.c bes_bsdiff_utils.c

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y := \
    -Iutils/bsdiff    \
    -Iplatform/drivers/ana \
	-Iutils/crc16   \
    -Iutils/crc32   \
    -Iutils/heap    \
    -Iutils/lzma    \
	-Iutils/boot_struct \

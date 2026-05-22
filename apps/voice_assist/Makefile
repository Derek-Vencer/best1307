cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
	-Iservices/multimedia/speech/inc \
	-Iservices/multimedia/speech/src/process/anc_assist/ \
	-Iservices/multimedia/audio/process/resample/include \
	-Iapps/audio_dump/include \
	-Iinclude/cmsis_dsp \
	-Iservices/multimedia/audio/process/anc/include \
	-Iapps/nv_section/aud_section \
	-Iapps/nv_section/include \
	-Iservices/app_ai/inc\
	-Imultimedia/inc/speech/inc\
	-Iservices/audio_bt

    .globl __bth_ram_bin_start__
    .globl __bth_ram_bin_end__
    .section .rodata
    .align 8
__bth_ram_bin_start__:
#if defined(CHIP_BEST1503)
    .incbin "../../services/../bthost/bthost_v1/rom/1503_t2/lib/bth_ram.bin"
#elif defined(CHIP_BEST1502X)
    .incbin "../../services/../bthost/bthost_v1/rom/1502x_simu/lib/bth_ram.bin"
#elif defined(CHIP_BEST1307)
    .incbin "../../services/../bthost/bthost_v1/rom/1307_simu/lib/bth_ram.bin"
#else
    #error this chip do not support bth-rom
#endif
__bth_ram_bin_end__:

cmd_drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o := /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/arm-none-linux-gnueabi-gcc -Wp,-MD,drivers/tfsr/PAM/Poseidon/.FSR_PAM_asm.o.d  -nostdinc -isystem /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -Iinclude  -I/home/jdubz/Downloads/BAKED/Kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-s5pv210/include -Iarch/arm/plat-s5p/include -Iarch/arm/plat-samsung/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float        -c -o drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.S

deps_drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o := \
  drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.S \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o: $(deps_drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o)

$(deps_drivers/tfsr/PAM/Poseidon/FSR_PAM_asm.o):

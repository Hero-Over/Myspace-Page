cmd_drivers/sensor/accel/smb380.o := /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/arm-none-linux-gnueabi-gcc -Wp,-MD,drivers/sensor/accel/.smb380.o.d  -nostdinc -isystem /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -Iinclude  -I/home/jdubz/Downloads/BAKED/Kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-s5pv210/include -Iarch/arm/plat-s5p/include -Iarch/arm/plat-samsung/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -pg -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack  -O2 -pipe -mcpu=cortex-a8 -mtune=cortex-a8 -mfloat-abi=soft -mfpu=neon  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(smb380)"  -D"KBUILD_MODNAME=KBUILD_STR(smb380)"  -c -o drivers/sensor/accel/smb380.o drivers/sensor/accel/smb380.c

deps_drivers/sensor/accel/smb380.o := \
  drivers/sensor/accel/smb380.c \
  drivers/sensor/accel/smb380.h \

drivers/sensor/accel/smb380.o: $(deps_drivers/sensor/accel/smb380.o)

$(deps_drivers/sensor/accel/smb380.o):

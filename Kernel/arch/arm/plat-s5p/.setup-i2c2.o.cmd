cmd_arch/arm/plat-s5p/setup-i2c2.o := /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/plat-s5p/.setup-i2c2.o.d  -nostdinc -isystem /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -Iinclude  -I/home/jdubz/Downloads/BAKED/Kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-s5pv210/include -Iarch/arm/plat-s5p/include -Iarch/arm/plat-samsung/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -pg -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack  -O2 -pipe -mcpu=cortex-a8 -mtune=cortex-a8 -mfloat-abi=soft -mfpu=neon  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(setup_i2c2)"  -D"KBUILD_MODNAME=KBUILD_STR(setup_i2c2)"  -c -o arch/arm/plat-s5p/setup-i2c2.o arch/arm/plat-s5p/setup-i2c2.c

deps_arch/arm/plat-s5p/setup-i2c2.o := \
  arch/arm/plat-s5p/setup-i2c2.c \
  include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/jdubz/Downloads/prebuilt/linux-x86/toolchain/arm-2009q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/posix_types.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  include/linux/typecheck.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/irqflags.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/hwcap.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/div64.h \
  arch/arm/mach-s5pv210/include/mach/gpio.h \
    $(wildcard include/config/s3c/gpio/space.h) \
    $(wildcard include/config/samsung/gpio/extra.h) \
    $(wildcard include/config/mach/s5pc110/aries.h) \
    $(wildcard include/config/s5pv210/victory.h) \
    $(wildcard include/config/s5pv210/atlas.h) \
    $(wildcard include/config/mach/aries.h) \
  include/asm-generic/gpio.h \
    $(wildcard include/config/gpiolib.h) \
    $(wildcard include/config/gpio/sysfs.h) \
    $(wildcard include/config/have/gpio/lib.h) \
  include/linux/errno.h \
  /home/jdubz/Downloads/BAKED/Kernel/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  arch/arm/mach-s5pv210/include/mach/victory/gpio-aries.h \
    $(wildcard include/config/aries/ver/b1.h) \
  arch/arm/plat-samsung/include/plat/iic.h \
  arch/arm/plat-samsung/include/plat/gpio-cfg.h \
    $(wildcard include/config/s5pv210/garnett/delta.h) \

arch/arm/plat-s5p/setup-i2c2.o: $(deps_arch/arm/plat-s5p/setup-i2c2.o)

$(deps_arch/arm/plat-s5p/setup-i2c2.o):
vpath %.s ../freertos/portable ./
vpath %.c ../app/src ../freertos/portable ../freertos/portable/gcc ../stm_std_lib/src  ../freertos ../cmsis/DeviceSupport ../cmsis/CoreSupport ../drivers
vpath %.h ../stm_std_lib/inc ../freertos/portable/gcc

CC :=arm-none-eabi-gcc
CXX :=arm-none-eabi-g++

DEFS += -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -DNDEBUG

INCS +=  -I../freertos/include -I../freertos/portable/gcc -I../app/inc -I../drivers

INCS += -I../stm_std_lib/inc -I../cmsis/DeviceSupport -I../cmsis/CoreSupport

SRC += $(wildcard ../stm_std_lib/src/*.c)
SRC += $(wildcard ../freertos/*.c)
SRC += $(wildcard ../freertos/portable/gcc/*.c)
SRC += $(wildcard ../cmsis/DeviceSupport/*.c)
SRC += $(wildcard ../cmsis/CoreSupport/*.c)
SRC += $(wildcard ../drivers/*.c)

OUTDIR := ./out
OBJS += $(patsubst %.c,%.o,$(SRC))
OBJS += startup_stm32f10x_md.o stm32f10x_it.o heap_4.o main.o
OUTOBJS +=$(patsubst %.o,$(OUTDIR)/%.o,$(notdir $(OBJS)))

CFLAGS += -mcpu=cortex-m3 -mthumb 
CFLAGS += -Os
CFLAGS += -ffunction-sections -fdata-sections

LFLAGS += -mcpu=cortex-m3 -mthumb
LFLAGS += -mfloat-abi=soft
LFLAGS += -Wl,--gc-sections 

.PHONY: all clean

all:$(OUTDIR)/blink.elf
	@arm-none-eabi-objcopy -O ihex -S -g $< $(OUTDIR)/out.hex
	@arm-none-eabi-objcopy $< -O binary -S -g $(OUTDIR)/out.bin
	@echo finished

$(OUTDIR)/blink.elf:$(OUTOBJS)
	@$(CC) $(LFLAGS) $(OUTOBJS) -T ./stm32_flash.ld -o $@
	@arm-none-eabi-size $@

clean:
	@rm -f $(OBJS) $(OBJ_FLASH) $(OBJ_SRAM)
	@rm -f out.hex blink.elf out.bin
	@rm -f $(OUTDIR)*.o

$(OUTDIR)/%.o:%.s
	@echo cc: $<
	@$(CC) $(CFLAGS) -c $< -o $@
$(OUTDIR)/%.o:%.c
	@echo cc: $<
	@$(CC) $(CFLAGS) $(DEFS) $(INCS) -c $< -o $@
$(OUTDIR)/%.o:%.cpp
	@echo cxx: $<
	@$(CXX) $(CFLAGS) $(DEFS) $(INCS) -c $< -o $@

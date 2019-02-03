STM32DIR ?= .
BOARD ?= f103-dev

TARGET_DIR ?= $(STM32DIR)/src/board/$(BOARD)/
include $(TARGET_DIR)/target.mk

TARGET_MCU ?= STM32F303RE

FLASH_OVERRIDE ?=
ifneq (,$(FLASH_OVERRIDE))
FLASH_HEX = $(shell python -c "print(hex($(FLASH_OVERRIDE) * 1024))")
endif

FLASH_RESERVE_PAGES ?= 0

ifneq (,$(findstring STM32F0, $(TARGET_MCU)))
TARGET_LINE = stm32f0
STARTUP_FILE ?= stm32f0
OPENOCD_TARGET = target/stm32f0x.cfg
OPENOCD_FLASH_DRIVER = stm32f1x
ARCH_FLAGS += -DSTM32F0
SYSTEM_FILE = system_stm32f0xx.c
ARCH_FLAGS += -mcpu=cortex-m0 -mthumb
LD_FLAGS = -specs=nano.specs -specs=nosys.specs --static -mthumb -mcpu=cortex-m0 -mfpu=vfp -ggdb3 -Wl,--gc-sections -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group 
endif
ifneq (,$(findstring STM32F1, $(TARGET_MCU)))
TARGET_LINE = stm32f1
STARTUP_FILE ?= stm32f1
OPENOCD_TARGET = target/stm32f1x.cfg
OPENOCD_FLASH_DRIVER = stm32f1x
ARCH_FLAGS += -DSTM32F1
SYSTEM_FILE = system_stm32f10x.c
ARCH_FLAGS += -mcpu=cortex-m3 -mthumb -mfpu=vfp -msoft-float -mfix-cortex-m3-ldrd
LD_FLAGS =  -specs=nano.specs -specs=nosys.specs --static -mthumb -mcpu=cortex-m3 -mfpu=vfp -ggdb3 -Wl,--gc-sections -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group 
endif
ifneq (,$(findstring STM32F3, $(TARGET_MCU)))
TARGET_LINE = stm32f3
STARTUP_FILE ?= stm32f350
OPENOCD_TARGET = $(STM32DIR)/target/stm32f3x.cfg
OPENOCD_FLASH_DRIVER = stm32f1x
ARCH_FLAGS += -DSTM32F3
SYSTEM_FILE = $(STM32DIR)/system_stm32f30x.c
ARCH_FLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -msoft-float -march=armv7e-m
LD_FLAGS =  -specs=nano.specs -specs=nosys.specs --static -mthumb -march=armv7e-m -mcpu=cortex-m4 -mfpu=vfp -ggdb3 -Wl,--gc-sections -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group 
endif

ifneq (,$(findstring STM32F0, $(TARGET_MCU)))

ifneq (,$(findstring STM32F030, $(TARGET_MCU)))
ifneq (,$(filter %C, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F030xC
else
ARCH_FLAGS += -DSTM32F030
endif
endif

ifneq (,$(findstring STM32F031, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F031
endif
ifneq (,$(findstring STM32F051, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F051
endif
ifneq (,$(findstring STM32F072, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F072
endif
ifneq (,$(findstring STM32F042, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F042
endif
ifneq (,$(findstring STM32F091, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F091
endif

ifneq (,$(findstring STM32F070, $(TARGET_MCU)))
ifneq (,$(filter %B, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F070xB
endif
ifneq (,$(filter %6, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F070x6
endif
endif

endif

ifneq (,$(findstring F103, $(TARGET_MCU)))
ifneq (,$(filter %4 %6, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F10X_LD
endif
ifneq (,$(filter %8 %B, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F10X_MD
endif
ifneq (,$(filter %C %D %E, $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F10X_HD
endif
endif

ifneq (,$(findstring stm32f3, $(TARGET_LINE)))
STM32F303xE_PARTS = STM32F303CE STM32F303CD STM32F303RE STM32F303RD STM32F303VE STM32F303VD STM32F303ZE STM32F303ZD STM32F302CE STM32F302CD STM32F302RE STM32F302RD STM32F302VE STM32F302ZE STM32F302ZD STM32F398VE
STM32F334x8_PARTS = STM32F334C4 STM32F334K4 STM32F334C6 STM32F334R6 STM32F334K6 STM32F334C8 STM32F334R8 STM32F334K8 STM32F303K8 STM32F303K6 STM32F303C8 STM32F303C6 STM32F303R8 STM32F303R6 STM32F328C8
STM32F350x8_PARTS = STM32F350G8
ifneq (,$(filter $(STM32F303xE_PARTS), $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F303xE
endif
ifneq (,$(filter $(STM32F334x8_PARTS), $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F334x8
endif
ifneq (,$(filter $(STM32F350x8_PARTS), $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F350x8
endif
endif

STM32LIB_DIR = $(STM32DIR)
CMSIS_DIR = $(STM32LIB_DIR)/driver/$(TARGET_LINE)/cmsis
SYSTEM_DIR = $(CMSIS_DIR)/device
STDPERIPH_DIR = $(STM32LIB_DIR)/driver/$(TARGET_LINE)/spl
LINK_DIR = $(STM32LIB_DIR)/src/link
STARTUP_DIR = $(STM32LIB_DIR)/src/startup

STARTUP_SRC = $(STARTUP_DIR)/startup_$(STARTUP_FILE).s
LINK_SCRIPT = $(LINK_DIR)/stm32.ld
SYSTEM_SRC = $(SYSTEM_DIR)/$(SYSTEM_FILE)

INCLUDES += -I $(SYSTEM_DIR)
INCLUDES += -I $(CMSIS_DIR)/core
INCLUDES += -I $(STDPERIPH_DIR)/inc
INCLUDES += -I $(STM32LIB_DIR)/src
INCLUDES += -I $(TARGET_DIR)

OPTIMIZE ?= -O0
CFLAGS = $(ARCH_FLAGS) 
CFLAGS += $(OPTIMIZE) -g -Wa,--warn -specs=nano.specs -fdata-sections -ffunction-sections
CFLAGS += $(INCLUDES)
CXXFLAGS ?=
CXXFLAGS += -std=gnu++14 $(ARCH_FLAGS)
CXXFLAGS += $(OPTIMIZE) -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs
CXXFLAGS += $(INCLUDES)
CXXFLAGS += $(BOARD_DEFINES)

CXX_SRC += $(wildcard $(STM32LIB_DIR)/src/*.cpp)

C_SRC += $(wildcard $(STDPERIPH_DIR)/src/*.c)
C_SRC += $(SYSTEM_SRC)

S_SRC += $(STARTUP_SRC)


LD_SRC += $(LINK_SCRIPT)
#LD_FLAGS += $(ARCH_FLAGS)

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AS = arm-none-eabi-as

BUILD_DIR ?= build
OBJ_DIR ?= $(BUILD_DIR)/obj
BIN_DIR ?= $(BUILD_DIR)/bin

TARGET_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.opp,$(CXX_SRC))
TARGET_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))
TARGET_OBJS += $(patsubst %.s,$(OBJ_DIR)/%.os,$(S_SRC))

$(OBJ_DIR)/%.o: %.c
	@echo "compiling $(notdir $<)"
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.opp: %.cpp
	@echo "compiling $(notdir $<)"
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

debug: $(TARGET_OBJS)
	@echo $(TARGET_OBJS)
	@echo $(OBJ_DIR)
	@echo $(ARCH_FLAGS)

#arm-none-eabi-g++ -c  -std=gnu++14 -DSTM32F1 -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp -msoft-float -mfix-cortex-m3-ldrd -DSTM32F10X_MD -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs-I ./driver/stm32f1/cmsis/device -I ./driver/stm32f1/cmsis/core -I ./driver/stm32f1/spl/inc -I ./src -o build/obj/./src/timer.opp src/timer.cpp
#arm-none-eabi-g++ -c  -std=gnu++14 -DSTM32F303xE -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs -I ./driver/stm32f3/cmsis/device -I ./driver/stm32f3/cmsis/core -I ./driver/stm32f3/spl/inc -I ./src -o build/obj/./src/timer.opp src/timer.cpp
example-%: $(TARGET_OBJS) $(OBJ_DIR)/src/example/example-%.opp
	@echo "deps: $^"
	@mkdir -p $(BIN_DIR)
ifneq (,$(FLASH_OVERRIDE))
	python $(STM32DIR)/src/link/generate-ldscript.py -p $(TARGET_MCU) -o$(FLASH_OVERRIDE) > $(STM32DIR)/src/link/stm32-mem.ld
else
	python $(STM32DIR)/src/link/generate-ldscript.py -p $(TARGET_MCU) > $(STM32DIR)/src/link/stm32-mem.ld
endif
	arm-none-eabi-g++ -o $(BIN_DIR)/$@.elf $^ -T $(STM32DIR)/src/link/stm32-mem.ld -T $(LD_SRC) $(LD_FLAGS)
	arm-none-eabi-size $(BIN_DIR)/$@.elf
	cp $(BIN_DIR)/$@.elf $(BIN_DIR)/debug.elf
	arm-none-eabi-objcopy -O ihex $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.hex
	cp $(BIN_DIR)/$@.hex $(BIN_DIR)/debug.hex
	arm-none-eabi-objcopy -O binary $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.bin

$(OBJ_DIR)/%.os: %.s
	echo "compiling $(notdir $<)"
	mkdir -p $(dir $@)
	$(CC) -c $(ASFLAGS) -o $@ $<

%-flash: %
ifneq (,$(FLASH_OVERRIDE))
	openocd \
	-f interface/stlink-v2.cfg \
	-f $(OPENOCD_TARGET) \
	-c "flash bank override $(OPENOCD_FLASH_DRIVER) 0x08000000 $(FLASH_HEX) 0 0 \$$_TARGETNAME" \
	-c "program build/bin/$<.hex verify reset exit" || \
	openocd \
	-f interface/stlink-v2-1.cfg \
	-f $(OPENOCD_TARGET) \
	-c "flash bank override $(OPENOCD_FLASH_DRIVER) 0x08000000 $(FLASH_HEX) 0 0 \$$_TARGETNAME" \
	-c "program build/bin/$<.hex verify reset exit"
else
	openocd -f interface/stlink-v2.cfg -f $(OPENOCD_TARGET) -c "program build/bin/$<.hex verify reset exit" || \
	openocd -f interface/stlink-v2-1.cfg -f $(OPENOCD_TARGET) -c "program build/bin/$<.hex verify reset exit"
endif

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)


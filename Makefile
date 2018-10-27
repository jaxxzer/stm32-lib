TARGET_MCU ?= STM32F303RE

FLASH_OVERRIDE ?=
ifneq (,$(FLASH_OVERRIDE))
FLASH_HEX = $(shell python -c "print(hex($(FLASH_OVERRIDE) * 1024))")
endif

ifneq (,$(findstring STM32F0, $(TARGET_MCU)))
TARGET_LINE = stm32f0
OPENOCD_TARGET = target/stm32f0x.cfg
OPENOCD_FLASH_DRIVER = stm32f1x
ARCH_FLAGS += -DSTM32F0
SYSTEM_FILE = system_stm32f0xx.c
ARCH_FLAGS += -DSTM32F030
ARCH_FLAGS += -mcpu=cortex-m0 -mthumb
endif
ifneq (,$(findstring STM32F1, $(TARGET_MCU)))
TARGET_LINE = stm32f1
OPENOCD_TARGET = target/stm32f1x.cfg
OPENOCD_FLASH_DRIVER = stm32f1x
ARCH_FLAGS += -DSTM32F1
SYSTEM_FILE = system_stm32f10x.c
ARCH_FLAGS += -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp -msoft-float -mfix-cortex-m3-ldrd
endif
ifneq (,$(findstring STM32F3, $(TARGET_MCU)))
TARGET_LINE = stm32f3
OPENOCD_TARGET = target/stm32f3x.cfg
OPENOCD_FLASH_DRIVER = stm32f3x
ARCH_FLAGS += -DSTM32F3
SYSTEM_FILE = system_stm32f30x.c
ARCH_FLAGS += -std=c99 -ggdb3 -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wextra -Wshadow -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes -fno-common -ffunction-sections -fdata-sections  -MD -Wall -Wundef
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
ifneq (,$(filter $(STM32F303xE_PARTS), $(TARGET_MCU)))
ARCH_FLAGS += -DSTM32F303xE
endif
endif

STM32LIB_DIR = .
CMSIS_DIR = $(STM32LIB_DIR)/driver/$(TARGET_LINE)/cmsis
SYSTEM_DIR = $(CMSIS_DIR)/device
STDPERIPH_DIR = $(STM32LIB_DIR)/driver/$(TARGET_LINE)/spl
LINK_DIR = $(STM32LIB_DIR)/src/link
STARTUP_DIR = $(STM32LIB_DIR)/src/startup

STARTUP_SRC = $(STARTUP_DIR)/startup_$(TARGET_LINE).s
LINK_SCRIPT = $(LINK_DIR)/stm32.ld
SYSTEM_SRC = $(SYSTEM_DIR)/$(SYSTEM_FILE)

INCLUDES += -I $(SYSTEM_DIR)
INCLUDES += -I $(CMSIS_DIR)/core
INCLUDES += -I $(STDPERIPH_DIR)/inc
INCLUDES += -I $(STM32LIB_DIR)/src

OPTIMIZE = -O0
CFLAGS = $(ARCH_FLAGS) 
CFLAGS += $(OPTIMIZE) -g -Wa,--warn -x assembler-with-cpp -specs=nano.specs
CFLAGS += $(INCLUDES)
CXXFLAGS ?=
CXXFLAGS += -std=gnu++14 $(ARCH_FLAGS)
CXXFLAGS += $(OPTIMIZE) -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs
CXXFLAGS += $(INCLUDES)

CXX_SRC += $(wildcard $(STM32LIB_DIR)/src/*.cpp)

C_SRC += $(wildcard $(STDPERIPH_DIR)/src/*.c)
C_SRC += $(SYSTEM_SRC)

S_SRC += $(STARTUP_SRC)


LD_SRC += $(LINK_SCRIPT)
LD_FLAGS =  -specs=nano.specs -specs=nosys.specs --static -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ggdb3 -Wl,--cref -Wl,--gc-sections -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group 
#LD_FLAGS += $(ARCH_FLAGS)

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AS = arm-none-eabi-as

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

TARGET_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.opp,$(CXX_SRC))
TARGET_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))
TARGET_OBJS += $(patsubst %.s,$(OBJ_DIR)/%.os,$(S_SRC))

$(OBJ_DIR)/%.o: %.c
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.opp: %.cpp
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

debug: $(TARGET_OBJS)
	@echo $(TARGET_OBJS)
	@echo $(OBJ_DIR)
	@echo $(ARCH_FLAGS)

#arm-none-eabi-g++ -c  -std=gnu++14 -DSTM32F1 -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp -msoft-float -mfix-cortex-m3-ldrd -DSTM32F10X_MD -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs-I ./driver/stm32f1/cmsis/device -I ./driver/stm32f1/cmsis/core -I ./driver/stm32f1/spl/inc -I ./src -o build/obj/./src/timer.opp src/timer.cpp
#arm-none-eabi-g++ -c  -std=gnu++14 -DSTM32F303xE -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs -I ./driver/stm32f3/cmsis/device -I ./driver/stm32f3/cmsis/core -I ./driver/stm32f3/spl/inc -I ./src -o build/obj/./src/timer.opp src/timer.cpp
example-%: $(TARGET_OBJS) $(OBJ_DIR)/src/example/example-%.opp
	@echo "deps: $^"
	mkdir -p $(BIN_DIR)
ifneq (,$(FLASH_OVERRIDE))
	python src/link/generate-ldscript.py -p $(TARGET_MCU) -o$(FLASH_OVERRIDE) > src/link/stm32-mem.ld
else
	python src/link/generate-ldscript.py -p $(TARGET_MCU) > src/link/stm32-mem.ld
endif
	arm-none-eabi-g++ -o $(BIN_DIR)/$@.elf $^ -T src/link/stm32-mem.ld -T $(LD_SRC) $(LD_FLAGS)
	arm-none-eabi-size $(BIN_DIR)/$@.elf
	cp $(BIN_DIR)/$@.elf $(BIN_DIR)/debug.elf
	arm-none-eabi-objcopy -O ihex $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.hex
	cp $(BIN_DIR)/$@.hex $(BIN_DIR)/debug.hex
	arm-none-eabi-objcopy -O binary $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.bin
	


$(OBJ_DIR)/%.os: %.s
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CC) -c $(ASFLAGS) -o $@ $<

%-flash: %
ifneq (,$(FLASH_OVERRIDE))
	openocd \
	-f interface/stlink-v2.cfg \
	-f $(OPENOCD_TARGET) \
	-c "flash bank override $(OPENOCD_FLASH_DRIVER) 0x08000000 $(FLASH_HEX) 0 0 \$$_TARGETNAME" \
	-c "program build/bin/$<.hex verify reset exit"
else
	openocd -f interface/stlink-v2.cfg -f $(OPENOCD_TARGET) -c "program build/bin/$<.hex verify reset exit"
endif

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)


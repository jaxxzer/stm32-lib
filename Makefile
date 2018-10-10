TARGET_MCU ?= STM32F103RC

ifneq (,$(findstring STM32F0, $(TARGET_MCU)))
TARGET_LINE = stm32f0
OPENOCD_TARGET = target/stm32f0x.cfg
ARCH_FLAGS += -DSTM32F0
SYSTEM_FILE = system_stm32f0xx.c
ARCH_FLAGS += -DSTM32F030
ARCH_FLAGS += -mcpu=cortex-m0 -mthumb
else
TARGET_LINE = stm32f1
OPENOCD_TARGET = target/stm32f1x.cfg
ARCH_FLAGS += -DSTM32F1
SYSTEM_FILE = system_stm32f10x.c
ARCH_FLAGS += -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp -msoft-float -mfix-cortex-m3-ldrd
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

OPTIMIZE = -Os
CFLAGS = $(ARCH_FLAGS) 
CFLAGS += $(OPTIMIZE) -g -Wa,--warn -x assembler-with-cpp -specs=nano.specs
CFLAGS += $(INCLUDES)
CXXFLAGS ?=
CXXFLAGS += -std=gnu++14 $(ARCH_FLAGS)
CXXFLAGS += $(OPTIMIZE) -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs
CXXFLAGS += $(INCLUDES) -DUSE_TIM_1 -DUSE_TIM_3

CXX_SRC += $(wildcard $(STM32LIB_DIR)/src/*.cpp)

C_SRC += $(wildcard $(STDPERIPH_DIR)/src/*.c)
C_SRC += $(SYSTEM_SRC)

S_SRC += $(STARTUP_SRC)

LD_SRC += $(LINK_SCRIPT)
LD_FLAGS = -mthumb -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=$(BUILD_DIR)/build.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x1000 -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -specs=nano.specs 
LD_FLAGS += $(ARCH_FLAGS)

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


example-%: $(TARGET_OBJS) $(OBJ_DIR)/src/example/example-%.opp
	@echo "deps: $^"
	mkdir -p $(BIN_DIR)
	python src/link/generate-ldscript.py -p $(TARGET_MCU) > src/link/stm32-mem.ld
	arm-none-eabi-g++ -o $(BIN_DIR)/$@.elf $^ -T src/link/stm32-mem.ld -T $(LD_SRC) $(LD_FLAGS)
	arm-none-eabi-size $(BIN_DIR)/$@.elf
	arm-none-eabi-objcopy -O ihex $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.hex
	arm-none-eabi-objcopy -O binary $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.bin
	
$(OBJ_DIR)/%.opp: %.cpp
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJ_DIR)/%.os: %.s
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CC) -c $(ASFLAGS) -o $@ $<

%-flash: %
	openocd -f interface/stlink-v2.cfg -f $(OPENOCD_TARGET) -c "program build/bin/$<.hex verify reset exit"
	cp $(BIN_DIR)/$<.elf $(BIN_DIR)/debug.elf

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)

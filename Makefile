TARGET_MCU ?= stm32f0
TARGET_FLASH ?= 16k

ifeq ($(TARGET_MCU), stm32f0)
ARCH_FLAGS += -DSTM32F030
#ARCH_FLAGS += -DSTM32F051
#ARCH_FLAGS += -DSTM32F072
ARCH_FLAGS += -mcpu=cortex-m0
endif
ARCH_FLAGS ?= -mcpu=cortex-m3

STM32LIB_DIR = .
CMSIS_DIR = $(STM32LIB_DIR)/driver/$(TARGET_MCU)/cmsis
SYSTEM_DIR = $(CMSIS_DIR)/device
STDPERIPH_DIR = $(STM32LIB_DIR)/driver/$(TARGET_MCU)/spl
LINK_DIR = $(STM32LIB_DIR)/src/link
STARTUP_DIR = $(STM32LIB_DIR)/src/startup

STARTUP_SRC = $(STARTUP_DIR)/startup_$(TARGET_MCU).s
LINK_SCRIPT = $(LINK_DIR)/stm32-$(TARGET_FLASH).ld
SYSTEM_SRC = $(SYSTEM_DIR)/system_$(TARGET_MCU)xx.c

INCLUDES = -I $(SYSTEM_DIR)
INCLUDES += -I $(CMSIS_DIR)/core
INCLUDES += -I $(STDPERIPH_DIR)/inc
INCLUDES += -I $(STM32LIB_DIR)/src

OPTIMIZE = -O0
CFLAGS = -mthumb $(ARCH_FLAGS) 
CFLAGS += -Os -g -Wa,--warn -x assembler-with-cpp -specs=nano.specs
CFLAGS += $(INCLUDES)
CXXFLAGS = -mthumb -std=gnu++14 $(ARCH_FLAGS)
CXXFLAGS += $(OPTIMIZE) -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -g -fstack-usage -Wall -fno-threadsafe-statics -specs=nano.specs
CXXFLAGS += $(INCLUDES) -DUSE_TIM_3
ASFLAGS = -mthumb $(ARCH_FLAGS)
ASFLAGS +=  -g -Wa,--warn -x assembler-with-cpp -specs=nano.specs
ASFLAGS += $(INCLUDES)
CXX_SRC += $(wildcard $(STM32LIB_DIR)/src/*.cpp)

 
C_SRC += $(wildcard $(STDPERIPH_DIR)/src/*.c)
C_SRC += $(SYSTEM_SRC)
S_SRC += $(STARTUP_SRC)
LD_SRC += $(LINK_SCRIPT)
LD_FLAGS = -mthumb -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=asf.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x1000 -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -specs=nano.specs 
LD_FLAGS += $(ARCH_FLAGS)


CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AS = arm-none-eabi-as

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

TARGET_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.opp,$(CXX_SRC))
TARGET_OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))
TARGET_OBJS += $(patsubst %.s,$(OBJ_DIR)/%.os,$(S_SRC))

$(OBJ_DIR)/%.o: %.c
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


example-%: $(TARGET_OBJS) $(OBJ_DIR)/src/example/example-%.opp
	@echo "deps: $^"
	arm-none-eabi-g++ -o $@.elf $^ -T $(LD_SRC) $(LD_FLAGS)
	arm-none-eabi-size $@.elf
	arm-none-eabi-objcopy -O ihex $@.elf $@.hex
	arm-none-eabi-objcopy -O binary $@.elf $@.bin

	
$(OBJ_DIR)/%.opp: %.cpp
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJ_DIR)/%.os: %.s
	@echo "dir $(dir $@)"
	mkdir -p $(dir $@)
	$(CC) -c $(ASFLAGS) -o $@ $<

%-flash: %
	openocd -f flash.cfg

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)

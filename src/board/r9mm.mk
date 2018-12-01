#sbus 1/3 A2
#sport/fport 1/6 AND 2/11 A5 and B10
#inverted sport
#ch1
#ch3
#ch3
#ch4
#red led
TARGET_MCU = STM32F103CB

STARTUP = stm32f1
BOARD_DEFINES += \
  -DGPIO_LED1_PORT=GPIOA \
  -DGPIO_LED1_PIN=5 \

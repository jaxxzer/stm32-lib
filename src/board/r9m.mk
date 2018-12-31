#sbus 1/3 A2
#sport/fport 1/6 AND 2/11 A5 and B10
#inverted sport 2/12 B11
#ch4 1/11 A10
#ch3 1/10 A9
#ch2 1/12 A11
#ch1 1/9 A8

#green led 1/13 A12
#red led 1/12 A11

#dip 1 A12
#dip 2 A11

#nrst 3/15 C14
#chip select 2/13 B12
#MOSI 2/16 B15
#MISO 2/13 AND 2/15 B12 and B14
#CK 2/14 B13

#cppm 1/1 A0 (via transistor)
#sport 2/11 B10 b11 via exti?

#buzzer 2/2 B1
#eeprom pin1 2/8

#power amplifier stage 1&2: 1/7 A6

#RF switch ctrl: B3

TARGET_MCU = STM32F103CB

STARTUP = stm32f1
BOARD_DEFINES += \
  -DUSE_TIM_1 \
  -DUSE_TIM_2 \
  -DGPIO_LED1_PORT=GPIOA \
  -DGPIO_LED1_PIN=11 \
  -DGPIO_LED1_TIMER=timer1 \
  -DGPIO_LED1_TIM_CH=TIM_Channel_4 \
  -DGPIO_LED1_REMAP=0 \
  -DUSE_USART_1 \
  -DSTDOUT_USART=uart1 \
  -DGPIO_USART1_TX_PORT=GPIOA \
  -DGPIO_USART1_TX_PIN=9 \
  -DGPIO_USART1_TX_MODE=GPIO_Mode_AF \
  -DGPIO_USART1_REMAP=0 \
  -DGPIO_USART1_RX_PORT=GPIOA \
  -DGPIO_USART1_RX_PIN=10 \
  -DGPIO_USART1_RX_MODE=GPIO_Mode_AF \
  -DGPIO_NRST_PORT=GPIOC \
  -DGPIO_NRST_PIN=14 \
   -DCAPTURE_TIMER=timer2 \
 -DGPIO_CAPTURE_PORT=GPIOA \
 -DGPIO_CAPTURE_PIN=0 \
 -DGPIO_CAPTURE_TIM_CH_RISING=TIM_Channel_1 \
 -DGPIO_CAPTURE_TIM_CH_FALLING=TIM_Channel_2 \
 -DGPIO_CAPTURE_REMAP=0 \
 -DUSE_EXTI

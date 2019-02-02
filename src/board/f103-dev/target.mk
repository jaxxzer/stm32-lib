TARGET_MCU = STM32F103C8


BOARD_DEFINES += \
 -DUSE_USART_1 \
 -DSTDOUT_USART=uart1 \
 -DGPIO_USART1_TX_PORT=GPIOA \
 -DGPIO_USART1_TX_PIN=9 \
 -DGPIO_USART1_TX_MODE=GPIO_Mode_AF \
 -DGPIO_USART1_REMAP=1 \
 -DGPIO_USART1_RX_PORT=GPIOA \
 -DGPIO_USART1_RX_PIN=10 \
 -DGPIO_USART1_RX_MODE=GPIO_Mode_AF \
 -DGPIO_LED1_MODE=GPIO_Mode_Out_PP \
 -DGPIO_LED1_TIMER=timer1 \
 -DGPIO_LED1_TIM_CH=TIM_Channel_1 \
 -DGPIO_LED1_REMAP=0
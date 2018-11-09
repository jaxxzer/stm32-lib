
TARGET_MCU = STM32F334R8

BOARD_DEFINES += \
 -DUSE_USART_1 \
 -DGPIO_USART1_TX_PORT=GPIOA \
 -DGPIO_USART1_TX_PIN=9 \
 -DGPIO_USART1_TX_AF=7 \
 -DGPIO_USART1_RX_PORT=GPIOA \
 -DGPIO_USART1_RX_PIN=10 \
 -DGPIO_USART1_RX_AF=7 \
 -DUSE_USART_2 \
 -DGPIO_USART2_TX_PORT=GPIOA \
 -DGPIO_USART2_TX_PIN=2 \
 -DGPIO_USART2_TX_AF=7 \
 -DGPIO_USART2_RX_PORT=GPIOA \
 -DGPIO_USART2_RX_PIN=3 \
 -DGPIO_USART2_RX_AF=7 \
 -DSTDOUT_USART=uart2 \
 -DGPIO_LED1_PORT=GPIOA \
 -DGPIO_LED1_PIN=5 \
 -DGPIO_LED1_MODE=GPIO_Mode_AF \
 -DGPIO_LED1_AF=1 \
 -DGPIO_LED1_TIMER=timer2 \
 -DGPIO_LED1_TIM_CH=TIM_Channel_1 \
 -DUSE_TIM_2

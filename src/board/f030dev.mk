
TARGET_MCU = STM32F030F4
FLASH_OVERRIDE = 32

BOARD_DEFINES += \
 -DGPIO_USART1_TX_PORT=GPIOA \
 -DGPIO_USART1_TX_PIN=9 \
 -DGPIO_USART1_TX_MODE=GPIO_Mode_AF \
 -DGPIO_USART1_TX_AF=1 \
 -DGPIO_USART1_RX_PORT=GPIOA \
 -DGPIO_USART1_RX_PIN=10 \
 -DGPIO_USART1_RX_MODE=GPIO_Mode_AF \
 -DGPIO_USART1_RX_AF=1 \
 -DGPIO_LED1_PORT=GPIOB \
 -DGPIO_LED1_PIN=1 \
 -DGPIO_LED1_MODE=GPIO_Mode_AF \
 -DGPIO_LED1_AF=1 \
 -DGPIO_LED1_TIMER=timer3 \
 -DGPIO_LED1_TIM_CH=TIM_Channel_4

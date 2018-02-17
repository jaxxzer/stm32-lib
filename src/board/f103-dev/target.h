// board defines for stm32f103 'blue pill'
// led is on pc13, which is recommended not recommended for led drives in the stm32 datasheet (funny)
// led also has no timer (also funny)
// :(

#define USE_USART_1
#define USE_TIM_1
#define USE_TIM_2
#define USE_TIM_3
#define STDOUT_USART uart1
#define GPIO_USART1_TX_PORT GPIOA
#define GPIO_USART1_TX_PIN 9
#define GPIO_USART1_TX_MODE GPIO_Mode_AF
#define GPIO_USART1_REMAP 1
#define GPIO_USART1_RX_PORT GPIOA
#define GPIO_USART1_RX_PIN 10
#define GPIO_USART1_RX_MODE GPIO_Mode_AF
#define GPIO_LED1_MODE GPIO_Mode_Out_PP
#define GPIO_LED1_TIMER timer1
#define GPIO_LED1_TIM_CH TIM_Channel_1
#define GPIO_LED1_REMAP 0
#define GPIO_LED1_PORT GPIOC
#define GPIO_LED1_PIN 13

#include "stm32lib-conf.h"

#define DSHOT 600

#define USART_BAUDRATE 230400

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };


void initUsart2(void)
{
    Gpio gpioUsart2Tx         { GPIO_USART2_TX_PORT, GPIO_USART2_TX_PIN };
    Gpio gpioUsart2Rx         { GPIO_USART2_RX_PORT, GPIO_USART2_RX_PIN };

#if defined(STM32F0)
    nvic_config(USART2_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART2_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart2Rx.init(GPIO_Mode_AF);
    gpioUsart2Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart2Rx.configAF(GPIO_USART2_RX_AF);
    gpioUsart2Tx.configAF(GPIO_USART2_TX_AF);
#elif defined(STM32F1)
    Gpio::remapConfig(GPIO_USART2_REMAP, ENABLE);
#else
 #error
#endif

    uart2.init(USART_BAUDRATE);
    uart2.ITConfig(USART_IT_RXNE, ENABLE);
    uart2.setEnabled(ENABLE);
    uart2.cls();
}

Timer& timerCapture = CAPTURE_TIMER;
Gpio gpioCapture { GPIO_CAPTURE_PORT, GPIO_CAPTURE_PIN };
TimerChannelInput tciRising { &timerCapture, GPIO_CAPTURE_TIM_CH_RISING };
TimerChannelInput tciFalling { &timerCapture, GPIO_CAPTURE_TIM_CH_FALLING };

const uint8_t numCaptures = 10;
uint16_t riseCapture, fallCapture, captures[numCaptures], captureIndex = 0;
uint32_t riseTime, fallTime;

const uint16_t dshot_bit_duration_ns = 1670;
const uint16_t dshot_1_high_duration_ns = 2 * dshot_bit_duration_ns / 2.675f;
const uint16_t dshot_0_high_duration_ns = 1 * dshot_bit_duration_ns / 2.675f;

const uint32_t dshot_tim_freq = 72000000;
const double dshot_tim_period = 1.0 / dshot_tim_freq;
const uint16_t dshot_1_tim_cnt = dshot_1_high_duration_ns / dshot_tim_period;
const uint16_t dshot_0_tim_cnt = dshot_0_high_duration_ns / dshot_tim_period;


void risingCallback(void)
{
    riseTime = microseconds;
    riseCapture = tciRising._peripheral->CCR1;
}

void fallingCallback(void)
{
    fallTime = microseconds;
    fallCapture = tciRising._peripheral->CCR2;
    captures[captureIndex++] = fallCapture - riseCapture;
    captureIndex = captureIndex % numCaptures;
}


int main()
{
    configureClocks(1000);
    initUsart2();
#if defined(STM32F1)
    gpioLed.init(GPIO_Mode_AF_PP);
    gpioCapture.init(GPIO_Mode_AF_OD);
    gpioLed.configRemap(GPIO_LED1_REMAP, ENABLE);
    gpioLed.configRemap(GPIO_CAPTURE_REMAP, ENABLE);
#elif defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_AF);
    gpioCapture.init(GPIO_Mode_AF);
    gpioLed.configAF(GPIO_LED1_AF);
    gpioCapture.configAF(GPIO_CAPTURE_AF);
#else
 #error
#endif

    timer.initFreq(1e3); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);

    timerCapture.setupCc1Callback(&risingCallback);
    timerCapture.setupCc2Callback(&fallingCallback);
    timerCapture.init(); // 1MHz clock frequency
    timerCapture.setEnabled(ENABLE);    
    timerCapture.interruptConfig(TIM_IT_CC1, ENABLE);
    timerCapture.interruptConfig(TIM_IT_CC2, ENABLE);

    // Note CCxS bits only writable when CCxE is 0 (channel is disabled)
    tciRising.init(TIM_ICPolarity_Falling, 0x0);
    tciFalling.init(TIM_ICPolarity_Rising, 0x0, TIM_ICPSC_DIV1, TIM_ICSelection_IndirectTI);



#if defined(STM32F0)
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
#elif defined(STM32F1)
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    nvic_config(TIM1_CC_IRQn, 0, 0, ENABLE);
#endif

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);
        //print_clocks();
        for ( uint8_t i = 0; i < numCaptures; i++) {
          printf("%d, ", captures[i]);

        }
        printf("\r\n");
        tco.setDuty(duty);
        if ( (inc > 0 && inc > 65535 - duty) ||
             (inc < 0 && duty < -inc) )
        {
            inc = -inc;
        }       
        duty += inc;
     }

    return 0;
}

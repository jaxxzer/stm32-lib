#include "stm32lib-conf.h"

#define DSHOT 600

#define USART_BAUDRATE 230400

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

#ifdef USE_USART_2
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
#endif USE_USART_2

Timer& timerCapture = CAPTURE_TIMER;
Gpio gpioCapture { GPIO_CAPTURE_PORT, GPIO_CAPTURE_PIN };


const uint8_t numCaptures = 50;
volatile uint16_t captures[numCaptures], fallCaptureIndex = 0;
volatile bool gotCapture = false;



void tifCallback(void)
{
}

int main()
{
    configureClocks(1000);
    //initUsart2();
#if defined(STM32F1)
    gpioLed.init(GPIO_Mode_AF_PP);
    gpioCapture.init(GPIO_Mode_AF_OD);
    gpioLed.remapConfig(GPIO_LED1_REMAP, ENABLE);
    gpioLed.remapConfig(GPIO_CAPTURE_REMAP, ENABLE);
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

    TIM_SelectInputTrigger(TIM1, TIM_TS_ETRF);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);

    timerCapture.setupTriggerCallback(&tifCallback);
    timerCapture.init(); // 1MHz clock frequency
    timerCapture.setEnabled(ENABLE);    
    timerCapture.interruptConfig(TIM_IT_Trigger, ENABLE);

#if defined(STM32F0)
    nvic_config(TIM2_IRQn, 0, ENABLE);
#elif defined(STM32F1)
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);
#endif
    //printf("hello\r\n");

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);

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

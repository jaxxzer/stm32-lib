#include "stm32lib-conf.h"

#define GPIO_USART1_TX      GPIOA
#define PIN_USART1_TX       9

#define GPIO_USART1_RX      GPIOA
#define PIN_USART1_RX       10

Gpio gpioUsart1Tx         { GPIO_USART1_TX, PIN_USART1_TX };
Gpio gpioUsart1Rx         { GPIO_USART1_RX, PIN_USART1_RX };

#if defined(STM32F0)
    Timer& timer = timer3;
    Gpio gpioLed { GPIOB, 1 };
    TimerChannelOutput tco { TIM3, TIM_Channel_4 };
#elif defined(STM32F1)
    Timer& timer = timer1;
    Gpio gpioLed { GPIOB, 13 };
    TimerChannelOutput tco { TIM1, TIM_Channel_1 };
#elif defined(STM32F3)
    Timer& timerPingDrive = timer3;
    Gpio gpioLed { GPIOC, 9 };
    Gpio gpioPingDrive { GPIOC, 6 };

    TimerChannelOutput tcoLed { TIM3, TIM_Channel_4 };
    TimerChannelOutput tcoPingDrive { TIM3, TIM_Channel_1 };

    Timer& timerPingInterval = timer1;
    TimerChannelOutput tcoPingDuration { TIM1, TIM_Channel_1 };
#else
#error
#endif

void startPing()
{
    tcoPingDrive.setEnabled(ENABLE);
    tcoLed.setEnabled(ENABLE);
}

void endPing()
{
    tcoPingDrive.setEnabled(DISABLE);
    tcoLed.setEnabled(DISABLE);
    uart1.write("Ping\r\n", 6);
}

void initGpio()
{
    gpioLed.init(GPIO_Mode_AF);
    gpioLed.configAF(2);

    gpioPingDrive.init(GPIO_Mode_AF);
    gpioPingDrive.configAF(2);
}

void initTimers()
{
    timerPingDrive.initFreq(115e3); // 115kHz pwm frequency
    timerPingDrive.setEnabled(ENABLE);
    timerPingDrive.setMOE(ENABLE);

    tcoPingDrive.init(TIM_OCMode_PWM1);
    tcoLed.init(TIM_OCMode_PWM1);

    tcoPingDrive.setDuty(UINT16_MAX / 2);
    tcoLed.setDuty(UINT16_MAX / 2);

#if defined(STM32F1) || defined(STM32F3)
    nvic_config(TIM1_UP_TIM16_IRQn, 0, 0, ENABLE);
    nvic_config(TIM1_CC_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

    timerPingInterval.interruptConfig(TIM_IT_Update, ENABLE);
    timerPingInterval.interruptConfig(TIM_IT_CC1, ENABLE);

    timerPingInterval.setupUpCallback(&startPing);
    timerPingInterval.setupCc1Callback(&endPing);

    timerPingInterval.initFreq(4); // 115kHz pwm frequency
    timerPingInterval.setEnabled(ENABLE);
    timerPingInterval.setMOE(ENABLE);

    tcoPingDuration.setCompare(1);
    tcoPingDuration.init(TIM_OCMode_PWM1);
}


void initUsart1(void)
{
#if defined(STM32F0) || defined(STM32F3)
	gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Rx.configAF(7);
    gpioUsart1Tx.configAF(7);
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#elif defined(STM32F1)
	gpioUsart1Rx.init(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpioUsart1Tx.init(GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
 #error
#endif
    uart1.init(115200);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}

int main()
{
    configureClocks(1000);

    initGpio();
    initUsart1();
    initTimers();

    while (1);

    return 0;
}

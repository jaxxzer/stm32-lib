// This example watches an input pin configured as AF_OD or INPUT_FLOATING (prefered?)
// Channel 1 gives the frequency (rise -> rise time)
// Channel 2 gives the pulse width

// packet structure
// uint8_t numChannels
// uint16[n] channel0...

#include "stm32lib-conf.h"
#include "sx1276.h"
#include "spi.h"

#define USART_BAUDRATE 230400

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

Gpio gpioReset { GPIO_NRST_PORT, GPIO_NRST_PIN };
Gpio gpioNss = { GPIOB, 12 };
Spi spi;

#if defined(USE_USART_1)
Gpio gpioUsart1Tx         { GPIO_USART1_TX_PORT, GPIO_USART1_TX_PIN };
Gpio gpioUsart1Rx         { GPIO_USART1_RX_PORT, GPIO_USART1_RX_PIN };
#endif

#if defined(USE_USART_2)
Gpio gpioUsart2Tx         { GPIO_USART2_TX_PORT, GPIO_USART2_TX_PIN };
Gpio gpioUsart2Rx         { GPIO_USART2_RX_PORT, GPIO_USART2_RX_PIN };
#endif

#if defined(USE_USART_3)
Gpio gpioUsart3Tx         { GPIO_USART3_TX_PORT, GPIO_USART3_TX_PIN };
Gpio gpioUsart3Rx         { GPIO_USART3_RX_PORT, GPIO_USART3_RX_PIN };
#endif

#if defined (USE_USART_1)
void initUsart1(void)
{
#if defined(STM32F0)
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
 #error
#endif



#if defined(STM32F0) || defined(STM32F3)
	gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
    gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
	gpioUsart1Rx.init(GPIO_Mode_AF_PP);
    gpioUsart1Tx.init(GPIO_Mode_AF_PP);
    Gpio::remapConfig(GPIO_USART1_REMAP, ENABLE);
#else
 #error
#endif

    uart1.init(USART_BAUDRATE);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}
#endif

#if defined (USE_USART_2)
void initUsart2(void)
{
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
#endif

#if defined(USE_USART_3)
void initUsart3(void)
{
#if defined(STM32F0)
    nvic_config(USART3_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART3_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart3Rx.init(GPIO_Mode_AF);
    gpioUsart3Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart3Rx.configAF(GPIO_USART3_RX_AF);
    gpioUsart3Tx.configAF(GPIO_USART3_TX_AF);
#elif defined(STM32F1)
    Gpio::remapConfig(GPIO_USART3_REMAP, ENABLE);
#else
 #error
#endif

    uart3.init(USART_BAUDRATE);
    uart3.ITConfig(USART_IT_RXNE, ENABLE);
    uart3.setEnabled(ENABLE);
    uart3.cls();
}
#endif



void initGpio()
{
    Gpio gpioMosi = { GPIOB, 15 };
    Gpio gpioMiso = { GPIOB, 14 };
    Gpio gpioSck = { GPIOB, 13 };

    gpioNss.init(GPIO_Mode_Out_PP);
    gpioMosi.init(GPIO_Mode_AF_PP);
    gpioMiso.init(GPIO_Mode_IN_FLOATING);
    gpioSck.init(GPIO_Mode_AF_PP);
            gpioNss.set();

    gpioReset.init(GPIO_Mode_Out_PP);
    gpioReset.set();
}
void resetDev()
{
    gpioReset.reset();
    mDelay(10);
    gpioReset.set();
    mDelay(10);
}


SX1276 sx1276(spi, gpioNss, gpioReset);


volatile uint16_t channels[16] = { 0 };
volatile uint16_t channelIndex = 0;
volatile uint16_t numChans = 0;
volatile bool updateAvailable = true;

Timer& timerCapture = CAPTURE_TIMER;
Gpio gpioCapture { GPIO_CAPTURE_PORT, GPIO_CAPTURE_PIN };
TimerChannelInput tciRising { &timerCapture, GPIO_CAPTURE_TIM_CH_RISING };
TimerChannelInput tciFalling { &timerCapture, GPIO_CAPTURE_TIM_CH_FALLING };

volatile uint16_t riseCapture;
volatile uint16_t fallCapture;
volatile uint32_t riseTime;
volatile uint32_t fallTime;

void printChannels(void) {
        for (uint8_t i = 0; i < numChans; i++) {
        printf("ch%d: %d\r\n", i, channels[i]);
    }
}
void risingCallback(void)
{
    riseCapture = tciRising._peripheral->CCR1;
    if (riseCapture > 2500) { // TODO needs to be more robust to deal with timer overflow during inter-frame period etc.
        updateAvailable = true;
        numChans = channelIndex;
        channelIndex = 0;

        return;
    }
    channels[channelIndex++] = riseCapture;
}

void fallingCallback(void)
{
    fallCapture = tciFalling._peripheral->CCR2;
}


// void writePacket(void) {
//     uint16_t buf[8];
//     buf[0] = numChans;
//     for (uint8_t i = 0; i < numChans; i++) {
//         buf[i + 1] = channels[i];
//     }
//     sx1276.transmit(buf, numChans * sizeof(uint16_t));
// }

int main()
{
    configureClocks(1000);
    initUsart1();
#if defined(STM32F1)
    gpioLed.init(GPIO_Mode_AF_PP);
    // gpioCapture.init(GPIO_Mode_AF_OD);
    gpioCapture.init(GPIO_Mode_IN_FLOATING);
    gpioLed.remapConfig(GPIO_LED1_REMAP, ENABLE);
    gpioCapture.remapConfig(GPIO_CAPTURE_REMAP, ENABLE);
#elif defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_AF);
    gpioCapture.init(GPIO_Mode_AF);
    gpioLed.configAF(GPIO_LED1_AF);
    gpioCapture.configAF(GPIO_CAPTURE_AF);
#else
 #error
#endif




    



    timer.initFreq(10e4); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);


    TIM_SelectInputTrigger(timerCapture.peripheral(), TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(timerCapture.peripheral(), TIM_SlaveMode_Reset);

    timerCapture.setupCc1Callback(&risingCallback);
    timerCapture.setupCc2Callback(&fallingCallback);
    timerCapture.init(47); // 1MHz clock frequency
    timerCapture.interruptConfig(TIM_IT_CC1, ENABLE);
    timerCapture.interruptConfig(TIM_IT_CC2, ENABLE);

    // Note CCxS bits only writable when CCxE is 0 (channel is disabled)
    tciRising.init(TIM_ICPolarity_Rising, 0x0);
    tciFalling.init(TIM_ICPolarity_Falling, 0x0, TIM_ICPSC_DIV1, TIM_ICSelection_IndirectTI);

    timerCapture.setEnabled(ENABLE);    


#if defined(STM32F0)
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
        nvic_config(TIM2_IRQn, 0, 0, ENABLE);

#elif defined(STM32F1)
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
        nvic_config(TIM2_IRQn, 0, 0, ENABLE);

#elif defined(STM32F3)
    nvic_config(TIM1_CC_IRQn, 0, 0, ENABLE);
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);
#endif


    initGpio();

    spi.init(SPI_BaudRatePrescaler_16);
    SPI_SSOutputCmd(SPI2, DISABLE);

    spi.enable(ENABLE);

        resetDev();
        sx1276.init();




        // while(1) {
        //     //sx1276.transmit("hello", 5);
        //     sx1276.transmit((char*)&inc, 4);
        //     inc++;
        //     gpioLed.toggle();
        // }
    



    // breath
    uint16_t duty = 0;
    int16_t inc = 1;
    tco.setDuty(30000);
    while (1) { 
        //mDelay(10);
        if (updateAvailable) {
                updateAvailable = false;
                sx1276.transmit((char*)channels, numChans * sizeof(channels[0]));

            //printChannels();
        }
        //print_clocks();
        //printf("T: %d, C: %d\r\n", (uint16_t)(riseCapture), (uint16_t)fallCapture);
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

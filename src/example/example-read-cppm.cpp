#include "stm32lib-conf.h"

#define DSHOT 600

#define USART_BAUDRATE 115200


typedef enum {
    DSHOT_CMD_MOTOR_STOP = 0,
    DSHOT_CMD_BEACON1,
    DSHOT_CMD_BEACON2,
    DSHOT_CMD_BEACON3,
    DSHOT_CMD_BEACON4,
    DSHOT_CMD_BEACON5,
    DSHOT_CMD_ESC_INFO, // V2 includes settings
    DSHOT_CMD_SPIN_DIRECTION_1,
    DSHOT_CMD_SPIN_DIRECTION_2,
    DSHOT_CMD_3D_MODE_OFF,
    DSHOT_CMD_3D_MODE_ON,
    DSHOT_CMD_SETTINGS_REQUEST, // Currently not implemented
    DSHOT_CMD_SAVE_SETTINGS,
    DSHOT_CMD_SPIN_DIRECTION_NORMAL = 20,
    DSHOT_CMD_SPIN_DIRECTION_REVERSED = 21,
    DSHOT_CMD_LED0_ON, // BLHeli32 only
    DSHOT_CMD_LED1_ON, // BLHeli32 only
    DSHOT_CMD_LED2_ON, // BLHeli32 only
    DSHOT_CMD_LED3_ON, // BLHeli32 only
    DSHOT_CMD_LED0_OFF, // BLHeli32 only
    DSHOT_CMD_LED1_OFF, // BLHeli32 only
    DSHOT_CMD_LED2_OFF, // BLHeli32 only
    DSHOT_CMD_LED3_OFF, // BLHeli32 only
    DSHOT_CMD_AUDIO_STREAM_MODE_ON_OFF = 30, // KISS audio Stream mode on/Off
    DSHOT_CMD_SILENT_MODE_ON_OFF = 31, // KISS silent Mode on/Off
    DSHOT_CMD_MAX = 47
} dshotCommands_e;

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

    Dma     dma1c7 = Dma(DMA1_Channel7);

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
Timer& timerCapture = CAPTURE_TIMER;
Gpio gpioCapture { GPIO_CAPTURE_PORT, GPIO_CAPTURE_PIN };
TimerChannelInput tciRising { &timerCapture, GPIO_CAPTURE_TIM_CH_RISING };
TimerChannelInput tciFalling { &timerCapture, GPIO_CAPTURE_TIM_CH_FALLING };
TimerChannelOutput tcoFraming { &timerCapture, TIM_Channel_3 };

const uint8_t numCaptures = 50;
volatile uint16_t riseCapture;
volatile uint16_t fallCapture;
volatile uint16_t fallCaptures[numCaptures], fallCaptureIndex = 0;
volatile uint16_t riseCaptures[numCaptures], riseCaptureIndex = 0;
volatile uint32_t riseTime, fallTime;

const uint16_t dshot_bit_duration_ns = 1670;
const uint16_t dshot_1_high_duration_ns = 2 * dshot_bit_duration_ns / 2.675f;
const uint16_t dshot_0_high_duration_ns = 1 * dshot_bit_duration_ns / 2.675f;

const uint32_t dshot_tim_freq = 72000000;
const double dshot_tim_period = 1.0 / dshot_tim_freq;
const uint16_t dshot_1_tim_cnt = dshot_1_high_duration_ns / dshot_tim_period;
const uint16_t dshot_0_tim_cnt = dshot_0_high_duration_ns / dshot_tim_period;


volatile uint16_t dshot_threshold = 250;
void risingCallback(void)
{
    // Period
    riseCapture = tciRising._peripheral->CCR1;
    riseCaptures[riseCaptureIndex++] = riseCapture;
    riseCaptureIndex = riseCaptureIndex % numCaptures;

}

void fallingCallback(void)
{
    // Duty cycle
    fallCapture = tciFalling._peripheral->CCR2;
    fallCaptures[fallCaptureIndex++] = fallCapture;
    fallCaptureIndex = fallCaptureIndex % numCaptures;
}

volatile bool gotCapture = false;
volatile uint16_t frameFallCaptures;
volatile uint16_t frameRiseCaptures;
volatile uint32_t decodedPackets = 0;
void cc3Callback(void)
{
    timerCapture.setEnabled(DISABLE);    
    frameFallCaptures = 50 - DMA1_Channel3->CNDTR;
    dshot_threshold = TIM1->CCR1 / 2;
    // timeout
    dma1c7.setEnabled(DISABLE);
    DMA1_Channel7->CNDTR = 50;
    dma1c7.setEnabled(ENABLE);
    decodedPackets++;
    //TIM_DMACmd(timerCapture.peripheral(), TIM_DMA_CC2, DISABLE);

    // frameFallCaptures = fallCaptureIndex;
    // frameRiseCaptures = riseCaptureIndex;

    // riseCaptureIndex = 0;
    // fallCaptureIndex = 0;

    gotCapture = true;
}

// ch1 -> rising counter is captured into ccr1 on rising edges tells you the frequency of pulse (ccr == time since last rising edge)
// ch2 -> falling timer counter is captured into ccr2 on falling edges
// ch3 -> reset and trigger timer on ccr match (make this less than or equal to the inter-frame period) Dma transfer is restarted (the buffer must be copied by application code before the next transfer request from the timer (next pulse))
int main()
{
    configureClocks(1000);
    initUsart1();
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

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
    
    //DBGMCU_APB2PeriphConfig(DBGMCU_TIM1_STOP, ENABLE);
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);


   // Using channel 2, ADC (when declared) uses channel 1
    dma1c7.init((uint32_t)&(TIM2->CCR2),
                (uint32_t)&fallCaptures[0],
                25,
                DMA_DIR_PeripheralSRC,
                DMA_PeripheralDataSize_HalfWord,
                DMA_MemoryDataSize_HalfWord,
                DMA_Mode_Normal,
                DMA_Priority_Medium,
                DMA_MemoryInc_Enable,
                DMA_PeripheralInc_Disable,
                DMA_M2M_Disable);

    //TIM1->DCR = TIM_DMABase_CCR1 | TIM_DMABurstLength_1Transfer;
    TIM_DMACmd(timerCapture.peripheral(), TIM_DMA_CC2, ENABLE);

    // timerCapture.setupCc1Callback(&risingCallback);
    // timerCapture.setupCc2Callback(&fallingCallback);
    timerCapture.setupCc3Callback(&cc3Callback);
    timerCapture.init(71); // 1MHz clock frequency
    //timerCapture.setEnabled(ENABLE);    
    // timerCapture.interruptConfig(TIM_IT_CC1, ENABLE);
    // timerCapture.interruptConfig(TIM_IT_CC2, ENABLE);
    timerCapture.interruptConfig(TIM_IT_CC3, ENABLE);

    // Note CCxS bits only writable when CCxE is 0 (channel is disabled)
    tcoFraming.init(TIM_OCMode_PWM1, 2000, TIM_OutputState_Enable);
    tciRising.init(TIM_ICPolarity_Rising, 0x0);
    tciFalling.init(TIM_ICPolarity_Falling, 0x0, TIM_ICPSC_DIV1, TIM_ICSelection_IndirectTI);
    dma1c7.setEnabled(ENABLE);
    timerCapture.setEnabled(ENABLE);
#if defined(STM32F0)
    nvic_config(TIM2_IRQn, 0, ENABLE);
#elif defined(STM32F1)
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);
#endif

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1000);
        printf("  %d    %d   %d\r\n", decodedPackets, microseconds, decodedPackets/microseconds/1000);

        //print_clocks();
        // if (gotCapture) {
        //     gotCapture = false;
        //     uint16_t packet = 0;
        //     uint16_t throttle = 0;
        //     uint8_t telemRequest = 0;
        //     uint8_t csum = 0;
        //     for (uint8_t i = 0; i < frameFallCaptures; i++) {
        //         bool bit = fallCaptures[i] > dshot_threshold;
        //         printf("%d ", fallCaptures[i]);
        //         //printf("%d", bit);
        //         packet = packet << 1;
        //         packet = packet | bit;
        //     }
        //     printf("  %d  %d  %d\r\n", packet, decodedPackets, microseconds);

        //     csum = packet & 0xf;
        //     packet = packet >> 4;
        //     telemRequest = packet & 0x1;
        //     throttle = packet >> 1;

        //     uint16_t csumCheck = 0;
        //     for (uint8_t i = 0; i < 3; i++) {
        //         csumCheck ^= packet;
        //         packet >>= 4;
        //     }

        //     csumCheck &= 0xf;


            
        //     //printf("%d ", frameFallCaptures);
        //     // for ( uint8_t i = 0; i < frameFallCaptures; i++) {
        //     //     printf("%d, ", fallCaptures[i]);
        //     // }
        //     //printf("%d ", frameRiseCaptures);
        //     // for ( uint8_t i = 0; i < frameRiseCaptures; i++) {
        //     //     printf("%d, ", riseCaptures[i]);
        //     // }
        //     printf("%d %d %d %d\r\n", throttle, telemRequest, csum, csumCheck);
        // }

        // tco.setDuty(duty);
        // if ( (inc > 0 && inc > 65535 - duty) ||
        //      (inc < 0 && duty < -inc) )
        // {
        //     inc = -inc;
        // }       
        // duty += inc;
     }

    return 0;
}

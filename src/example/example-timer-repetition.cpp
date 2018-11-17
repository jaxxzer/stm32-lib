#include "stm32lib-conf.h"

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

uint16_t pulses[] = { 100, 250, 100, 100, 250, 250, 100, 100, 100, 250, 250, 250, 0};
int main()
{
    configureClocks(1000);

    timer.initFreq(1); // 1Hz update frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
    
    timerCapture.init(); // 1MHz clock frequency
    timerCapture.setEnabled(ENABLE);    

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);
        //print_clocks();
        if (gotCapture) {
            gotCapture = false;
            uint16_t packet = 0;
            uint16_t throttle = 0;
            uint8_t telemRequest = fallCaptures[11] > 250;
            uint8_t csum = 0;
            for (uint8_t i = 0; i < 16; i++) {
                bool bit = fallCaptures[i] > 250;
                //printf("%d", bit);
                packet = packet << 1;
                packet |= bit;
            }

            csum = packet & 0xf;
            packet = packet >> 4;
            telemRequest = packet & 0x1;
            throttle = packet >> 1;

            uint16_t csumCheck = 0;
            for (uint8_t i = 0; i < 3; i++) {
                csumCheck ^= packet;
                packet >>= 4;
            }

            csumCheck &= 0xf;


            
            //printf("%d ", frameFallCaptures);
            // for ( uint8_t i = 0; i < frameFallCaptures; i++) {
            //     printf("%d, ", fallCaptures[i]);
            // }
            //printf("%d ", frameRiseCaptures);
            // for ( uint8_t i = 0; i < frameRiseCaptures; i++) {
            //     printf("%d, ", riseCaptures[i]);
            // }
            printf("%d %d %d %d\r\n", throttle, telemRequest, csum, csumCheck);
        }

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

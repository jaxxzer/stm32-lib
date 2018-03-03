#pragma once

#include "gpio.h"
#include "LowPassFilter.h"
class Led;

/// BLHeli_32 Wraith
/// TIM1: PWM and master control of motor phases
/// CH1:
/// CH2:
/// CH3:
/// CH4:
/// TIM2: LED
/// - CH2: Green PB3 AF
/// TIM3: LED
/// - CH1: Blue PB4 AF
/// TIM4: Commutation


/// ADC1: Only ADC available on this M0
/// - CH0: pA0 AF Phase C
/// - CH1: pA1 AF Virtual Neutral
/// - CH2: pA2 AF Input Signal
/// - CH3: pA3 AF Voltage Measurement
/// - CH4: pA4 AF Phase A
/// - CH5: pA5 AF Phase B
/// - CH6: pA6 AF Current Measuerment


#define COM_MASK1      (TIM_CCx_Enable << TIM_Channel_1)
#define COM_MASK2      (TIM_CCx_Enable << TIM_Channel_2)
#define COM_MASK3      (TIM_CCx_Enable << TIM_Channel_3)

#define COM_MASK1N     (TIM_CCxN_Enable << TIM_Channel_1)
#define COM_MASK2N     (TIM_CCxN_Enable << TIM_Channel_2)
#define COM_MASK3N     (TIM_CCxN_Enable << TIM_Channel_3)

#define COM_MASK_NONE  0
#define COM_MASK_ALL   COM_MASK1 | COM_MASK2 | COM_MASK3 | COM_MASK1N | COM_MASK2N | COM_MASK3N

#define COM_MASK_STEP0 COM_MASK1  | COM_MASK1N | COM_MASK2N | COM_MASK3
#define COM_MASK_STEP1 COM_MASK1  | COM_MASK2N | COM_MASK3  | COM_MASK3N
#define COM_MASK_STEP2 COM_MASK1N | COM_MASK2  | COM_MASK3  | COM_MASK3N
#define COM_MASK_STEP3 COM_MASK1N | COM_MASK2  | COM_MASK2N | COM_MASK3
#define COM_MASK_STEP4 COM_MASK1  | COM_MASK2  | COM_MASK2N | COM_MASK3N
#define COM_MASK_STEP5 COM_MASK1  | COM_MASK1N | COM_MASK2  | COM_MASK3N

Adc adcA(ADC1);

//static const uint8_t magPoles = 12;
uint8_t volume = 10;
class Brushless
{
public:
    Brushless(void);
    Uart* usart1;


    bool setRpmTarget(const uint32_t rpm_target); // mHz
    uint32_t getRpmTarget(void);
    uint32_t getRpm(void);

    void playStartupTune();
    void playNote(uint16_t, uint16_t);
    void commutate(void);

    void initialize(void);

    // Hardware specific peripheral initialization
    void usartInit(void);
    void ledInit(void);

    void init3PhaseOutput(void);

    // Flags
    volatile bool initialized;
	volatile bool armed;
	volatile bool running;

	void HallHandler(uint16_t capture_time);
	void setVolume(uint16_t volume);

    Gpio* high1;
    Gpio* high2;
    Gpio* high3;
    Gpio* low1;
    Gpio* low2;
    Gpio* low3;

    Pwm* pwm1;
    Pwm* pwm2;
	Pwm* pwm3;

	Pwm* pwmR;
	Pwm* pwmG;
	Pwm* pwmB;
 void analogInToFreq(void);
	Gpio* hall1;
	LowPassFilter<int32_t> mHz;
//	Gpio hall2;
//	Gpio hall3;

	AdcChannel* adcPhaseC;
	AdcChannel* adcPhaseNeutral;
	AdcChannel* adcInput;
	AdcChannel* adcVoltage;
	AdcChannel* adcPhaseA;
	AdcChannel* adcPhaseB;
	AdcChannel* adcCurrent;
	void adcInit(void);
	void hallInit(void);

//	Led* ledRed;
//	Led* ledGreen;
//	Led* ledBlue;

	Timer* pwmTimer;
	Timer* commutationTimer;
	Timer* greenLedTimer;

	void setRGB(uint8_t g);

	void waitThrottleLow() {
		adcInput->waitValue(0, 10); // value, margin
	}

	void allLow(void);

	// Updated @ 1000Hz
	uint16_t voltage(void);
	uint16_t current(void);
	volatile uint32_t hallCount;


private:
    void advanceCommutation(bool reverse=false);
    uint32_t _rpm;
    uint32_t _rpmTarget;
    volatile uint8_t state; // 3 bits


    void noOutput(void);
    void audioStatePreload(void);
    void commutationStatePreload(void);
};

Brushless* b = new Brushless();

Brushless::Brushless()
	: initialized(false)
	, running(false)
	, armed(false)
	, pwmTimer(NULL)
	, commutationTimer(NULL)
	, greenLedTimer(NULL)
	, hallCount(0)
{
	mHz.set_cutoff_frequency(1.0f);
}

void Brushless::initialize(void)
{
	usartInit();
	usart1->cls();
	printf("\n\rInitializing Wraith32");
	printf("\n\r\t- SystemCoreClock: %d", SystemCoreClock);
	ledInit();
	adcInit();

	hallInit();
	init3PhaseOutput();
	audioStatePreload();
	playStartupTune();
//	allLow();

	while (1) {
		adcA.waitConversion();
		analogInToFreq();
	}

}

void Brushless::usartInit(void) {
	usart1 = new Uart(USART1);
	usart1->init();
}

void Brushless::adcInit(void)
{
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~ ADC configuration ~~~~~~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////


	adcPhaseC = adcA.addChannel(ADC_Channel_0); // pA0, Phase C
	adcPhaseNeutral = adcA.addChannel(ADC_Channel_1); // pA1, Virtual Neutral
	adcInput = adcA.addChannel(ADC_Channel_2); // pA2, Input Signal
	adcVoltage = adcA.addChannel(ADC_Channel_3); // pA3, Voltage Measurement
	adcPhaseA = adcA.addChannel(ADC_Channel_4); // pA4, Phase A
	adcPhaseB = adcA.addChannel(ADC_Channel_5); // pA5, Phase B
	adcCurrent = adcA.addChannel(ADC_Channel_6); // PA6, Current Measurement

	adcA.calibrate();
	adcA.enable();
	adcA.DmaConfig();
	adcA.startConversion();
	adcA.waitConversion();
}

void Brushless::hallInit(void) {
	printf("\n\r\t- Initializing Hall Sensor");

	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~ HALL configuration ~~~~~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
	Gpio pB5 = Gpio(GPIOB, 5);
	pB5.initInPullUp();
	pB5.initAFPP();
	pB5.configAF(1);

	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		NVIC_InitTypeDef NVIC_InitStructure;

		/* Enable the TIM3 global Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
		//	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		//	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		TIM_ICInitTypeDef  TIM_ICInitStructure;
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICFilter = 0x04;

		TIM_ICInit(TIM3, &TIM_ICInitStructure);

		TIM_TimeBaseInitTypeDef timerInitStructure;
		timerInitStructure.TIM_Prescaler = 20000; // Everything is in microseconds
		timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		timerInitStructure.TIM_Period = 65000; // AKA ARR, period in ticks
		timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		timerInitStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM3, &timerInitStructure);
		TIM_Cmd(TIM3, ENABLE);
		TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	}
}

////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
////~~~~~~~~~~~~~~~~~~~ LED configuration ~~~~~~~~~~~~~~~////
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
void Brushless::ledInit(void)
{
	printf("\n\r\t- Initializing LED indicator 1");
	greenLedTimer = new Timer(TIM2);
	Gpio pB3 = Gpio(GPIOB, 3);
	pB3.configAF(2);
	pwmG = new Pwm(&pB3, greenLedTimer, TIM_Channel_2);
	pwmG->init();
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	greenLedTimer->setPeriod(2048);
	pwmG->setDutyCycle(2040);
}

void Brushless::init3PhaseOutput(void) {
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
	////~~~~~~~~~~ Commutation Timer Configuration ~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////

	if (initialized) {
		printf("WARNING: Initialize called more than once");
		return;
	}
	printf("\n\rInit 3 Phase Output");


	if (!pwmTimer) {
		pwmTimer = new Timer(TIM1);
		// fatal; should never get here
	}
	//	TimerChannel* a = phaseTimer.initChannel(1);
	//	a.initComplement();
	//	TimerChannel* b = phaseTimer.initChannel(2);
	//	b.initComplement();
	//	TimerChannel* a = phaseTimer.initChannel(3);
	//	b.initComplement();

	low1 = new Gpio(GPIOA, 7); // TIM1_CH1N
	high1 = new Gpio(GPIOA, 8);  // TIM1_CH1
	pwm1 = new Pwm(high1, pwmTimer, TIM_Channel_1);

	low2 = new Gpio(GPIOB, 0); // TIM1_CH2N
	high2 =  new Gpio(GPIOA, 9);  // TIM1_CH2
	pwm2 =  new Pwm(high2, pwmTimer, TIM_Channel_2);

	low3 = new Gpio(GPIOB, 1); // TIM1_CH3N
	high3 = new Gpio(GPIOA, 10); // TIM1_CH3
	pwm3 = new Pwm(high3, pwmTimer, TIM_Channel_3);

	low1->initAFPP();
	pwm1->initComplimentary();
	low2->initAFPP();
	pwm2->initComplimentary();
	low3->initAFPP();
	pwm3->initComplimentary();

	TIM1->BDTR |= 0x8800;

	noOutput();

	low1->configAF(2);
	low2->configAF(2);
	low3->configAF(2);

	high1->configAF(2);
	high2->configAF(2);
	high3->configAF(2);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	initialized = true;
}

void Brushless::noOutput(void) {
	if (!pwmTimer) {
		return;
	}
	TIM_ITConfig(pwmTimer->peripheral(), TIM_IT_CC1, DISABLE);
	TIM_ITConfig(pwmTimer->peripheral(), TIM_IT_CC2, DISABLE);
	TIM_ITConfig(pwmTimer->peripheral(), TIM_IT_CC3, DISABLE);
	TIM_ITConfig(pwmTimer->peripheral(), TIM_IT_CC4, DISABLE);
//	allLow();
}

void Brushless::allLow(void)
{
	//TIM_Cmd(commutationTimer->peripheral(), DISABLE);
	TIM_SelectOCxM(pwmTimer->peripheral(), TIM_Channel_1, TIM_ForcedAction_InActive);
	TIM_SelectOCxM(pwmTimer->peripheral(), TIM_Channel_2, TIM_ForcedAction_InActive);
	TIM_SelectOCxM(pwmTimer->peripheral(), TIM_Channel_3, TIM_ForcedAction_InActive);
	pwmTimer->peripheral()->CCER = COM_MASK1N | COM_MASK2N | COM_MASK3N;
	armed = false;
	running = false;
}

void Brushless::setVolume(uint16_t volume) {
	pwm2->setDutyCycle(300 + volume);
	pwm3->setDutyCycle(300 + volume);
}

void Brushless::audioStatePreload(void)
{
	pwmTimer->outputChannelInitPwmComplimentary(TIM_Channel_4, 62);

	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef STM32F10X_MD
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		//		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
		//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//		NVIC_Init(&NVIC_InitStructure);
	#else
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0; // audio priority commutation has second highest priority after reading input
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	#endif

	// 24kHz = 1/24000 s = 41.667 microseconds = period
	// prescalar = 1 = 8MHz = 125 nanoseconds
	//	phaseTimer.setPrescaler(50); // to slow the waveform down 50x
	pwmTimer->setFrequency(1800); // ARR adjustment in frequency terms

	/// During Audio:
	/// CCR1 = 30
	/// CCR2 = 500
	/// CCR3 = 500
	/// CCR1 = 35 // should be less but takes a long time to switch for some reason (this takes 51.5 microseconds currently)

	pwm1->setDutyCycle(200);
	pwm2->setDutyCycle(300);
	pwm3->setDutyCycle(300);
	TIM_SetCompare4(TIM1, 100); // short pulse next time

	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);

	//TODO set other members here
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRStructInit(&TIM_BDTRInitStructure);
	TIM_BDTRInitStructure.TIM_DeadTime = 156; // 2.0 us // 0x02 for 250ns
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	  TIM1->CCER = COM_MASK1 | COM_MASK1N | COM_MASK2 | COM_MASK3;

	TIM1->BDTR |= 0x8800;
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
}

void Brushless::commutationStatePreload(void)
{


}

void Brushless::commutate(void) {
	//            -----------------------------------------------
	//           | Step0 | Step1 | Step2 | Step3 | Step4 | Step5 |
	//----------------------------------------------------------
	//|Channel1  |   1   |   0   |   0   |   0   |   0   |   1   |
	//----------------------------------------------------------
	//|Channel1N |   1   |   0   |   1   |   1   |   0   |   1   |
	//----------------------------------------------------------
	//|Channel2  |   0   |   0   |   0   |   1   |   1   |   0   |
	//----------------------------------------------------------
	//|Channel2N |   1   |   1   |   0   |   1   |   1   |   0   |
	//----------------------------------------------------------
	//|Channel3  |   0   |   1   |   1   |   0   |   0   |   0   |
	//----------------------------------------------------------
	//|Channel3N |   0   |   1   |   1   |   0   |   1   |   1   |


	//            -----------------------------------------------
	//           | Step0 | Step1 | Step2 | Step3 | Step4 | Step5 |
	//-----------------------------------------------------------
	//|Channel1  |‾|_|‾|_________________________________|‾|_|‾|_|
	//-----------------------------------------------------------
	//|Channel1N |_|‾|_|‾|_______|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|_________|‾|_|‾|
	//-----------------------------------------------------------
	//|Channel2  |_______________________|‾|_|‾|_|‾|_|‾|_________|
	//-----------------------------------------------------------
	//|Channel2N |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|_________|‾|_|‾|_|‾|_|‾|_______|
	//-----------------------------------------------------------
	//|Channel3  |_______|‾|_|‾|_|‾|_|‾|_________________________|
	//-----------------------------------------------------------
	//|Channel3N |_________|‾|_|‾|_|‾|_|‾|_______|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|


	//CCPC = 1!!!! 17.4.9
	//            -----------------------------------------------
	//           | Step0 | Step1 | Step2 | Step3 | Step4 | Step5 |
	//-----------------------------------------------------------
	//|CC1E      |   1   |   1   |   0   |   0   |   1   |   1   |
	//-----------------------------------------------------------
	//|CC1NE     |   1   |   0   |   1   |   1   |   0   |   1   |
	//-----------------------------------------------------------
	//|OC1M      |  110  |  100  |  101  |  101  |  100  |  110  |
	//-----------------------------------------------------------
	//|CC2E      |   0   |   0   |   1   |   1   |   1   |   1   |
	//-----------------------------------------------------------
	//|CC2NE     |   1   |   1   |   0   |   1   |   1   |   0   |
	//-----------------------------------------------------------
	//|OC2M      |  101  |  101  |  100  |  110  |  110  |  100  |
	//-----------------------------------------------------------
	//|CC3E      |   1   |   1   |   1   |   1   |   0   |   0   |
	//-----------------------------------------------------------
	//|CC3NE     |   0   |   1   |   1   |   0   |   1   |   1   |
	//-----------------------------------------------------------
	//|OC3M      |  100  |  110  |  110  |  100  |  101  |  101  |
	//-----------------------------------------------------------

	TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

	state = (state + 1) % 6; // 3 bits, represent phases
	switch (state) {
	case 0:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive); // Sense 3
		TIM1->CCER = COM_MASK1 | COM_MASK1N | COM_MASK2N | COM_MASK3;
		break;
	case 1:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_InActive);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_Active); // Sense 1
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM1->CCER = COM_MASK1 | COM_MASK2N | COM_MASK3 | COM_MASK3N;
		break;
	case 2:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_InActive); // Sense 2
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM1->CCER = COM_MASK1N | COM_MASK2 | COM_MASK3 | COM_MASK3N;
		break;
	case 3:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive);
		TIM1->CCER = COM_MASK1N | COM_MASK2 | COM_MASK2N | COM_MASK3;
		break;
	case 4:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_InActive);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_Active);
		TIM1->CCER = COM_MASK1 | COM_MASK2 | COM_MASK2N | COM_MASK3N;
		break;
	case 5:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_InActive);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_Active);
		TIM1->CCER = COM_MASK1 | COM_MASK1N | COM_MASK2 | COM_MASK3N;
		break;
	default:
		break;
	}
}

// Check state
void Brushless::playStartupTune(void) {
//	uint16_t volume = 0;
//
//	for (uint8_t j = 0; j < 5; j++) {
//
//		for (uint8_t i = 0; i < 10; i++) {
//			playNote(1000 + i * 400, 20);
//					setVolume(volume);
//					volume = volume + 10;
//
//		}
//		for (uint8_t i = 10; i > 0; i--) {
//			playNote(1000 + i * 400, 20);
//			setVolume(volume);
//			volume = volume + 10;
//		}
//	}

	uint16_t volume = 1300;

	for (uint8_t j = 0; j < 50; j++) {

		for (uint8_t i = 0; i < 3; i++) {
			playNote(1000 + i * 400, 100);
					setVolume(volume);
					if (volume >= 40) {
					volume = volume - volume / 15.0f;
					} else {
						goto done;
					}

		}

	}
//	playNote(3000, 1000);
	playNote(800, 1000);

	done:
	setVolume(1300);

//	while(1);
//	noOutput();
}

void Brushless::analogInToFreq(void) {
	uint16_t f = map(adcInput->_average, 0, 4095, 800, 6000);
	pwmTimer->setFrequency(f);
}

void Brushless::playNote(uint16_t frequency, uint16_t duration_ms)
{
	if (!pwmTimer) {
		return;
	}
	pwmTimer->playNote(frequency, duration_ms);
}

inline void Brushless::HallHandler(uint16_t captureTime) {
	static uint32_t last = 0;
	const uint32_t t = MicroSeconds;
	const float dt = (t - last) / 1000000.0f;
	last = t;

	usart1->cls();
	printf("\rCount: %d \tRPM: %d\tCapture: %d\tin: %d", ++hallCount, mHz.apply(captureTime, dt), captureTime, adcInput->_average);

	if (!pwmG) {
		return;
	}
	static bool state = false;
	state = !state;
	pwmG->setDutyCycle(state * 49 + 2000);

}
volatile bool flip = true;

/// ~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@
/// Interrupts
extern "C" {

	/// ~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	/// CC2: Handler for Hall sensor input capture
	/// Update:
	void TIM3_IRQHandler(void)
	{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2)) {
			TIM3->CNT = 0;

			b->HallHandler(TIM3->CCR2);

			TIM_ClearFlag(TIM3, TIM_FLAG_CC2);
		} else if (TIM_GetITStatus(TIM3, TIM_IT_Update)) {
			TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		}
	}

	/// ~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	/// CC2: left phase for audio state
	/// CC3: right phase for audio state
	/// CC4: offset timing counter for audio state
	/// Update:
	void TIM1_CC_IRQHandler(void) {
		//printf("\n\r%d", TIM1->SR);
		if (TIM_GetITStatus(TIM1, TIM_IT_CC2)) {
			TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_InActive);
			TIM1->SR = (uint16_t)~TIM_IT_CC2;
		} else if (TIM_GetITStatus(TIM1, TIM_IT_CC3)){
			TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive);
			flip = !flip;
			TIM1->SR = (uint16_t)~TIM_IT_CC3;
		} else if (TIM_GetITStatus(TIM1, TIM_IT_CC4)) {
			if (flip) {
				TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
			} else {
				TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
			}
			TIM1->SR = (uint16_t)~TIM_IT_CC4;
		}
	}
}


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

/// TIM6:
/// - Commutation

/// TIM14:
/// TIM15:
/// TIM16
/// TIM17:




/// ADC1: Only ADC available on this M0
/// - CH0: pA0 AF Phase C
/// - CH1: pA1 AF Virtual Neutral
/// - CH2: pA2 AF Input Signal
/// - CH3: pA3 AF Voltage Measurement
/// - CH4: pA4 AF Phase A
/// - CH5: pA5 AF Phase B
/// - CH6: pA6 AF Current Measuerment



/// Priorities:
/// - Input updates
///   - Commutation
///   - Audio Switching
///     - Hall update/ADC Update


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

//#define UINT16_MAX 1<<16
//#define INT16_MAX 1<<15

//static const uint8_t magPoles = 12;
uint8_t volume = 10;
class Brushless
{
public:
    Brushless(void);

	void HallHandler(uint16_t capture_time);
	void setDutyCycle(uint16_t duty);

	// Members

	// Constants
	static const uint16_t _volume_max = 1300;
	static const uint16_t _volume_min = 500;
	static const uint16_t _rotor_poles = 12;

    // Flags
    volatile bool initialized;
	volatile bool armed;
	volatile bool running;

	LowPassFilter<int32_t> period;

	uint16_t pwmFrequency;
	float pwmPeriod;
	volatile uint32_t hallCount;

	uint16_t _rpm;

	AdcChannel* adcPhaseC;
//	AdcChannel* adcPhaseNeutral;
	AdcChannel* adcInput;
	AdcChannel* adcVoltage;
//	AdcChannel* adcPhaseA;
//	AdcChannel* adcPhaseB;
	AdcChannel* adcCurrent;

    Uart usart1 { USART1 };

	Gpio gpio_LedG  { GPIOB, 3  };
	Gpio gpio_LedB  { GPIOB, 4  };
    Gpio gpio_High1 { GPIOA, 8  };
    Gpio gpio_High2 { GPIOA, 9  };
    Gpio gpio_High3 { GPIOA, 10 };
    Gpio gpio_Low1  { GPIOA, 7  };
    Gpio gpio_Low2  { GPIOB, 0  };
    Gpio gpio_Low3  { GPIOB, 1  };
    Gpio gpio_Hall  { GPIOB, 5  };

    TimerChannelOutput tco_pwm1 { TIM1, TIM_Channel_1 };
    TimerChannelOutput tco_pwm2 { TIM1, TIM_Channel_2 };
    TimerChannelOutput tco_pwm3 { TIM1, TIM_Channel_3 };
    TimerChannelOutput tco_pwm4 { TIM1, TIM_Channel_4 };

	TimerChannelOutput tco_LedG { TIM2, TIM_Channel_2 };
	TimerChannelOutput tco_LedB { TIM3, TIM_Channel_1 };

	TimerChannelInput tci_Hall { TIM3, TIM_Channel_2 };

	Timer tim_Pwm  { TIM1 };
	Timer tim_Com  { TIM6 };
	Timer tim_LedG { TIM2 };
	Timer tim_LedB_Hall { TIM3 };

	//////// Startup /////////
    void initialize(void);
    void setupCommutationTimer(void);

	void adcInit(void);
	void hallInit(void);
    void init3PhaseOutput(void);

    // Hardware specific peripheral initialization
    void usartInit(void);
    void ledInit(void);

	//////// API /////////
	void analogInToFreq(void);
	void update(void);
	void setRGB(uint8_t g);
	void waitThrottleLow() {
//		adcInput.waitValue(0, 10); // value, margin
	}

	void startup();
	void setVolume(uint16_t volume); //0~max;
	void allLow(void);
    bool setRpmTarget(const uint32_t rpm_target); // mHz
    uint32_t getRpmTarget(void);
    uint32_t getRpm(void);

    void playStartupTune();
    void playNote(uint16_t, uint16_t);
    void commutate(void);

	// Updated @ 1000Hz
	uint16_t voltage(void);
	uint16_t current(void);


private:
    void advanceCommutation(bool reverse=false);
    uint32_t _rpmTarget;
    volatile uint8_t state; // 3 bits

    void noOutput(void);
    void audioStatePreload(void);
    void commutationStatePreload(void);
};

Brushless b = Brushless();


Brushless::Brushless()
	: initialized(false)
	, running(false)
	, armed(false)
	, hallCount(0)
{
	period.set_cutoff_frequency(5.0f);
	pwmFrequency = 16000;

}

void Brushless::initialize(void)
{
	usartInit();
	usart1.cls();
	printf("\n\rInitializing Wraith32");
	printf("\n\r\t- SystemCoreClock: %lu", SystemCoreClock);
	ledInit();
	adcInit();
	hallInit();
	init3PhaseOutput();
	audioStatePreload();
	playStartupTune();

	// TODO TImer.disable, Timerchanel.disable, and refactore these mehtods to just "disarmed"
	noOutput();
	allLow();
	while (1) {
		tco_LedG.setCompare(200 * gpio_Hall.readInput());
		tco_LedB.setCompare(200 * !gpio_Hall.readInput());
	}


	commutationStatePreload();

//	setupCommutationTimer();
	while (1) {
		update();
//		commutate();
//		analogInToFreq();
	}

}

void Brushless::usartInit(void) {
	usart1 = Uart(USART1);
	usart1.init();
}

void Brushless::setupCommutationTimer(void) {


	tim_Com.setClockEnabled(ENABLE);
	tim_Com.setPrescaler(15);
	tim_Com.init();

//	tim_Com.setFrequency(750);


	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_StructInit(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	//	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}

//void Brushless::startupMotor(void)
//{
//	bool statup_condition = adcInput->average > 0;
//}

void Brushless::adcInit(void)
{

	adcPhaseC = adcA.addChannel(ADC_Channel_0); // pA0, Phase C
	//adcPhaseNeutral = adcA.addChannel(ADC_Channel_1); // pA1, Virtual Neutral
	adcInput = adcA.addChannel(ADC_Channel_2); // pA2, Input Signal
	adcVoltage = adcA.addChannel(ADC_Channel_3); // pA3, Voltage Measurement
	//adcPhaseA = adcA.addChannel(ADC_Channel_4); // pA4, Phase A
	//adcPhaseB = adcA.addChannel(ADC_Channel_5); // pA5, Phase B
	adcCurrent = adcA.addChannel(ADC_Channel_6); // PA6, Current Measurement

	adcA.calibrate();
	adcA.enable();
	adcA.DmaConfig();
	adcA.startConversion();
	adcA.waitConversion();
}

void Brushless::hallInit(void) {
	printf("\n\r\t- Initializing Hall Sensor");

	//////// Set up hall sensor and interrrupt
	gpio_Hall.init(GPIO_Mode_AF, GPIO_PuPd_UP);
	gpio_Hall.configAF(1);

	tim_LedB_Hall.init(100, 2048);
	tim_LedB_Hall.setEnabled(ENABLE); // redundant

	tci_Hall.init(TIM_ICPolarity_BothEdge, 0x04); // always enabled?

	nvic_config(TIM3_IRQn, 1, ENABLE);
	tim_LedB_Hall.interruptConfig(TIM_IT_CC2, ENABLE);
}

////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
////~~~~~~~~~~~~~~~~~~~ LED configuration ~~~~~~~~~~~~~~~////
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
void Brushless::ledInit(void)
{
	printf("\n\r\t- Initializing LED indicator 1");
	//////// Set up green led
	gpio_LedG.init(GPIO_Mode_AF);
	gpio_LedG.configAF(2);

	tim_LedG.init(0, 2048); // prescaler, arr
	tim_LedG.setEnabled(ENABLE);

	tco_LedG.init(TIM_OCMode_PWM1, 100, TIM_OutputState_Enable); // Only works after enabling

	printf("\n\r\t- Initializing LED indicator 2");
	//////// Set up blue led
	gpio_LedB.init(GPIO_Mode_AF);
	gpio_LedB.configAF(1);

	tim_LedB_Hall.init(0, 2048); // note we are utilizing a single timer for simultaneous input and output on seperate channels
	tim_LedB_Hall.setEnabled(ENABLE);
	tco_LedB.init(TIM_OCMode_PWM1, 100, TIM_OutputState_Enable); // Only works after enabling
}

void Brushless::init3PhaseOutput(void) {
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////
	////~~~~~~~~~~ Commutation Timer Configuration ~~~~~~~~~~////
	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~////

	if (initialized) {
		printf("WARNING: Initialize called more than once");
		return;
	}
	printf("\n\r\t- Initalizing 3 Phase Output");

	tim_Pwm.init(0, 2048); // prescaler, arr
	tim_Pwm.setEnabled(ENABLE);

	tco_pwm1.init(TIM_OCMode_PWM1, 100, TIM_OutputState_Enable, TIM_OutputNState_Enable);
	tco_pwm2.init(TIM_OCMode_PWM1, 100, TIM_OutputState_Enable, TIM_OutputNState_Enable);
	tco_pwm3.init(TIM_OCMode_PWM1, 100, TIM_OutputState_Enable, TIM_OutputNState_Enable);

    gpio_High1.init(GPIO_Mode_AF);
    gpio_High2.init(GPIO_Mode_AF);
    gpio_High3.init(GPIO_Mode_AF);
    gpio_Low1.init(GPIO_Mode_AF);
    gpio_Low2.init(GPIO_Mode_AF);
    gpio_Low3.init(GPIO_Mode_AF);

    gpio_High1.configAF(2);
    gpio_High2.configAF(2);
    gpio_High3.configAF(2);
    gpio_Low1.configAF(2);
    gpio_Low2.configAF(2);
    gpio_Low3.configAF(2);

	TIM1->BDTR |= 0x8800;

	noOutput();
	allLow();

	tim_Pwm.setMOE(ENABLE);

	initialized = true;
}

void Brushless::noOutput(void) {
	tim_Pwm.ITConfig(TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 , DISABLE);
}

void Brushless::allLow(void)
{
	//TIM_Cmd(commutationTimer.peripheral(), DISABLE);
	TIM_SelectOCxM(tim_Pwm.peripheral(), TIM_Channel_1, TIM_ForcedAction_InActive);
	TIM_SelectOCxM(tim_Pwm.peripheral(), TIM_Channel_2, TIM_ForcedAction_InActive);
	TIM_SelectOCxM(tim_Pwm.peripheral(), TIM_Channel_3, TIM_ForcedAction_InActive);
	tim_Pwm.peripheral()->CCER = COM_MASK1N | COM_MASK2N | COM_MASK3N;
	armed = false;
	running = false;
}

void Brushless::setVolume(uint16_t volume) {
	uint16_t v = map(volume, 0, UINT16_MAX, _volume_min, _volume_max);
	tco_pwm2.setCompare(v);
	tco_pwm3.setCompare(v);
}

void Brushless::audioStatePreload(void)
{
	#ifdef STM32F10X_MD
	NVIC_InitTypeDef NVIC_InitStructure;

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
		nvic_config(TIM1_CC_IRQn, 0, ENABLE);
	#endif

	// 24kHz = 1/24000 s = 41.667 microseconds = period
	// prescalar = 1 = 8MHz = 125 nanoseconds
	//	phaseTimer.setPrescaler(50); // to slow the waveform down 50x
	tim_Pwm.setFrequency(1800); // ARR adjustment in frequency terms

	/// During Audio:
	/// CCR1 = 30
	/// CCR2 = 500
	/// CCR3 = 500
	/// CCR1 = 35 // should be less but takes a long time to switch for some reason (this takes 51.5 microseconds currently)

	tco_pwm1.setCompare(200);
	tco_pwm2.setCompare(300);
	tco_pwm3.setCompare(300);
	tco_pwm4.setCompare(100);

	tim_Pwm.interruptConfig(TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

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

	allLow();
	setDutyCycle(0);
//	pwmTimer.peripheral().EGR |= 1;
//	while (pwmTimer.peripheral().EGR & 1); // neccessary/useful/safe?


	tim_Pwm.setFrequency(pwmFrequency); // ARR adjustment in frequency terms
	pwmPeriod = 1000000.0f / pwmFrequency; // microseconds
	TIM1->BDTR = 0x8800 | 0b0001010;

	printf("\n\rSet pwm frequency to: %d Hz", pwmFrequency);
	// TODO derive actual resolution with dead time consideraitons
	printf("\n\rThis results in a period of %d microseconds and %lu ARR value", (uint16_t)(pwmPeriod), tim_Pwm.peripheral()->ARR);
	printf("\n\rDTG is %d", tim_Pwm.peripheral()->BDTR & 0x7F);
	setDutyCycle(0);
}

void Brushless::setDutyCycle(uint16_t duty) {
	//TODO use TimerChannel dutycycle
	uint16_t dutyCycle = map(duty, 0, 1<<12, 0, tim_Pwm.peripheral()->ARR);
	tco_pwm1.setCompare(dutyCycle);
	tco_pwm1.setCompare(dutyCycle);
	tco_pwm1.setCompare(dutyCycle);
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

	state = (state + 1) % 2; // 3 bits, represent phases
	switch (state) {
	case 0:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive);

		TIM1->CCER = COM_MASK1 | COM_MASK1N | COM_MASK2N | COM_MASK3;
		break;
	case 1:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive);

		TIM1->CCER = COM_MASK1N | COM_MASK2 | COM_MASK2N | COM_MASK3;
//		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_InActive);
//		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_Active); // Sense 1
//		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
//		TIM1->CCER = COM_MASK1 | COM_MASK2N | COM_MASK3 | COM_MASK3N;
		break;
	case 2:
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_Active);
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_InActive); // Sense 2
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM1->CCER = COM_MASK1N | COM_MASK2 | COM_MASK3 | COM_MASK3N;
		break;
	case 3:

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
	uint16_t volume = UINT16_MAX;
	static const uint8_t numNotes = 20;
	static const uint8_t scaleNotes = 3;

	printf("\n\r\t - Playing startup tune |");

	for (uint8_t j = 0; j < numNotes/scaleNotes; j++) {
		for (uint8_t i = 0; i < scaleNotes; i++) {
			playNote(1000 + i * 400, 100);
			setVolume(volume);
			if (volume >= 10000) {
				volume = volume - volume / 20.0f;
			} else {
				goto done;
			}
		}
	}
	done: {}
}

void Brushless::update(void)
{
	adcA.waitConversion();

	setDutyCycle(adcInput->_average);

	static uint32_t tNow = 0;
	static uint32_t tLastInput = 0;
	static uint32_t tLastRpm = 0;
	static const uint32_t inputUpdatePeriod = 500;
	static const uint32_t rpmUpdatePeriod = 200;
	tNow = MicroSeconds;

	if (tNow > tLastInput + inputUpdatePeriod) {
		tLastInput = tNow;
		printf("{\"Input\":%d,\"Voltage\":%d,\"Current\":%d}", adcInput->_average, adcVoltage->_average, adcCurrent->_average);
	}
	if (tNow > tLastRpm + rpmUpdatePeriod) {
		tLastRpm = tNow;
		uint16_t p = period.get(); // in TIM counts

		/// p = TIM3 counts
		/// TIM3 counts = TIM3 clock 48MHz / TIM3 prescaler 1000 = 48000Hz
	    /// TIM3 counts / 48000 = t time in seconds between magnets passing by
		/// _rotor_poles * t = tr time in seconds for one rotation
		/// frequency = 1/t
		/// ----------
		//		static const float rotation_period = _rotor_poles * p / 48000 ;
				 float rotation_frequency = 48000000.0f / (_rotor_poles * p); // x1000!

		printf("{\"Period\":%d,\"Hz\":%d}", p, (uint32_t)(rotation_frequency));
	}
}

void Brushless::analogInToFreq(void) {
//	uint16_t f = map(adcInput._average, 0, 4095, 800, 6000);
//	pwmTimer.setFrequency(f);
}

void Brushless::playNote(uint16_t frequency, uint16_t duration_ms)
{
	static uint8_t playState;
	static const char status[] = { '|', '/', '-', '\\' };
	usart1.bkspc();
	usart1.write(status[playState++%4]);
	tim_Pwm.setFrequency(frequency);
	DelayMil(duration_ms);
}

inline void Brushless::HallHandler(uint16_t captureTime) {
	static uint32_t last = 0;
	const uint32_t t = MicroSeconds;
	const float dt = (t - last) / 1000000.0f; // seconds
	static uint16_t f = 1/dt;
	last = t;

	++hallCount;

	period.apply(captureTime, dt);

	static bool state = false;
	state = !state;
	tco_LedG.setCompare(state * 49 + 2000);

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

//		b.commutate();
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2)) {
			printf("hello");

//			TIM3->CNT = 0;
//			b.HallHandler(TIM3->CCR2);
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

	void TIM6_DAC_IRQHandler(void)
	{
		printf("hello");
//		b.commutate();
//		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	}
}


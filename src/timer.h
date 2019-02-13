#pragma once
#include "stm32lib-conf.h"

class Timer {
public:
  Timer(TIM_TypeDef *timx) : _peripheral(timx) {
    setClockEnabled(ENABLE);
    TIM_TimeBaseStructInit(&_config);
  };

  ~Timer() {
    // delete callbacks
    _deleteCallbacks();
  }

  // apply current config (TIM_TimeBaseInitTypeDef)
  void setAutoreload(uint32_t arr);
  void _initTimeBase(void);
  void _irqHandler(void);

  void _executeCallbacks(it_callback_t *callbacks);

  void _deleteCallbacks(void);

  // apply default (startup) bdtr configuration
  void _initBDTR(void);
  // apply current BDTR configuration
  void _configBDTR();
  // this has separate handling than the rest of btdr bits in stm spl due to.. safety? or maybe this cannot be written
  // synchronously with other bits
  void setMOE(FunctionalState newState) { TIM_CtrlPWMOutputs(_peripheral, newState); }
  void setOSSR(FunctionalState newState) {
    _config_bdtr.TIM_OSSRState = newState ? TIM_OSSRState_Enable : TIM_OSSRState_Disable;
    _configBDTR();
  }
  void setOSSI(FunctionalState newState) {
    _config_bdtr.TIM_OSSIState = newState ? TIM_OSSIState_Enable : TIM_OSSIState_Disable;
    _configBDTR();
  }
  void setDTG(uint16_t deadTime) {
    _config_bdtr.TIM_DeadTime = deadTime;
    _configBDTR();
  } // in nanoseconds

  // Defaults
  void init(uint16_t prescaler = 0x0000, uint32_t period = 0xFFFFFFFF, uint16_t counterMode = TIM_CounterMode_Up,
            uint16_t clockDivision = TIM_CKD_DIV1, uint8_t repetitionCounter = 0x0000);

  void initFreq(uint32_t frequency) {
    init();
    setFrequencyForce(frequency);
  }

  // contorl/config
  void setEnabled(FunctionalState enabled); // enable output
  void setClockEnabled(FunctionalState enabled);
  void preloadConfig(FunctionalState enabled);
  void setRepetitionCounter(uint16_t repetitions);
  void setCCPreloadControl(FunctionalState enabled);
  uint32_t getAutoreload(void);
  bool setFrequency(uint32_t f);
  bool setFrequencyForce(uint32_t f);
  void generateEvent(uint16_t TIM_EventSource);
  //	void setPeriod(uint32_t microseconds);
  //    uint16_t getPeriod(void);

  void clearFlag(uint16_t flag) { TIM_ClearFlag(_peripheral, flag); };
  void setPrescaler(uint16_t prescaler);

  TIM_TypeDef *peripheral(void) { return _peripheral; };

  // Interrupt configuration and handling
  void interruptConfig(const uint8_t interrupts, const FunctionalState newState);

  // todo private
  it_callback_t *upCallbacks = nullptr;
  it_callback_t *triggerCallbacks = nullptr;
  it_callback_t *cc1Callbacks = nullptr;
  it_callback_t *cc2Callbacks = nullptr;
  it_callback_t *cc3Callbacks = nullptr;
  it_callback_t *cc4Callbacks = nullptr;

  // todo private
  it_callback_t *addCallback(it_callback_t **callbacks, void (*newCallbackFn)(void));
  it_callback_t *setupUpCallback(void (*upCallbackFn)(void));
  it_callback_t *setupTriggerCallback(void (*triggerCallbackFn)(void));
  it_callback_t *setupCc1Callback(void (*cc1CallbackFn)(void));
  it_callback_t *setupCc2Callback(void (*cc2CallbackFn)(void));
  it_callback_t *setupCc3Callback(void (*cc3CallbackFn)(void));
  it_callback_t *setupCc4Callback(void (*cc4CallbackFn)(void));

private:
  TIM_TypeDef *_peripheral;
  TIM_TimeBaseInitTypeDef _config;
  TIM_BDTRInitTypeDef _config_bdtr // todo add constructor
      {
          TIM_OSSRState_Enable,       // TIM_OSSRState = 0;
          TIM_OSSIState_Disable,      // TIM_OSSIState = 0;
          TIM_LOCKLevel_OFF,          // TIM_LOCKLevel = 0;
          0,                          // TIM_DeadTime = 0; 0 ~ 0xFF
          TIM_Break_Disable,          // TIM_Break = 0;
          TIM_BreakPolarity_Low,      // TIM_BreakPolarity = 0;
          TIM_AutomaticOutput_Disable // TIM_AutomaticOutput = 0;
      };
};

class TimerChannel {
public:
  TimerChannel(Timer *timer, uint8_t channel) : _peripheral(timer->peripheral()), _channel(channel){};
  TIM_TypeDef *_peripheral;
  uint8_t _channel;
};

class TimerChannelInput : public TimerChannel {
public:
  TimerChannelInput(Timer *timer, uint8_t channel) : TimerChannel(timer, channel) { TIM_ICStructInit(&_config); };

  void setEnabled(FunctionalState enabled);
  void setICPolarity(uint16_t polarity);
  void setICSelection(uint16_t selection);
  void setICPrescaler(uint16_t prescaler);
  void setICFilter(uint16_t filter);

  // Defaults
  void init(uint16_t polarity = TIM_ICPolarity_Rising, uint16_t filter = 0x00, uint16_t prescaler = TIM_ICPSC_DIV1,
            uint16_t selection = TIM_ICSelection_DirectTI);

  // Current config
  void _init(void);

  FunctionalState _enabled; // TODO: extend TIM_ICInitTypeDef to support this too

  TIM_ICInitTypeDef _config;
};

class TimerChannelOutput : public TimerChannel {
public:
  TimerChannelOutput(Timer *timer, uint8_t channel) : TimerChannel(timer, channel) { TIM_OCStructInit(&_config); };

  void setMode(uint16_t mode);

  void setDuty(uint16_t duty); // 0 ~ UINT16MAX
  void setCompare(uint16_t ccr);

  void preloadConfig(FunctionalState enabled);
  void setEnabled(FunctionalState enabled);
  void setEnabledN(FunctionalState enabled);

  void setPolarity(FunctionalState high);
  void setPolarityN(FunctionalState high);

  void setIdleState(FunctionalState high);
  void setIdleStateN(FunctionalState high);

  // Current config
  void init(uint16_t ocMode = TIM_OCMode_Timing, uint32_t pulse = 0x0000000 // aka CCR
            ,
            uint16_t outputState = TIM_OutputState_Disable, uint16_t outputNState = TIM_OutputNState_Disable,
            uint16_t ocpolarity = TIM_OCPolarity_High, uint16_t ocNpolarity = TIM_OCNPolarity_High,
            uint16_t ocIdleState = TIM_OCIdleState_Reset, uint16_t ocNIdleState = TIM_OCNIdleState_Reset);
  void _init(void);

  TIM_OCInitTypeDef _config;
};

// see stm32f05x datasheet 3.14 Timers and watchdogs
#ifdef USE_TIM_1
extern Timer timer1; // 16 bit Advanced control
#endif
#ifdef USE_TIM_2
extern Timer timer2; // 32 bit General purpose
#endif
#ifdef USE_TIM_3
extern Timer timer3; // 16 bit Advanced control
#endif
#ifdef USE_TIM_6
extern Timer timer6; // 16 bit Basic
#endif
#ifdef USE_TIM_14
extern Timer timer14; // 16 bit Advanced control
#endif
#ifdef USE_TIM_15
extern Timer timer15; // 16 bit Advanced control
#endif
#ifdef USE_TIM_16
extern Timer timer16; // 16 bit Advanced control
#endif
#ifdef USE_TIM_17
extern Timer timer17; // 16 bit Advanced control
#endif

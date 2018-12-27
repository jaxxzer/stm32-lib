#pragma once
#include "stm32f10x_exti.h"

class Exti
{
public:
    Exti() = default;

    void initLine(uint32_t line = EXTI_Line0,
        EXTIMode_TypeDef mode = EXTI_Mode_Interrupt,
        EXTITrigger_TypeDef trigger = EXTI_Trigger_Falling,
        FunctionalState state = ENABLE)
    {

    }

    static const uint8_t numExti = 16;
    it_callback_t* _callbacks[numExti] = { nullptr };

    it_callback_t* setupCallback(uint8_t exti, void (*newCallbackFn)(void)) {
        
        it_callback_t** callbacks = &_callbacks[exti];
        it_callback_t* newCb = new it_callback_t;
        newCb->callback = newCallbackFn;
        it_callback_t* currentCb = *callbacks;
        if (!currentCb) {
            *callbacks = newCb;
        } else {
            it_callback_t* tail = *callbacks;
            while (tail->next != nullptr) {
                tail = tail->next;
            }
            tail->next = newCb;
        }
        return newCb;
    }

    void _irqHandler(uint8_t exti)
    {
        if (exti >= numExti) {
            return;
        }
        _executeCallbacks(_callbacks[exti]);
    }

    void _executeCallbacks(it_callback_t* callbacks)
    {
        it_callback_t* cb = callbacks;
        while (cb) {
            cb->callback();
            cb = cb->next;
        }
    } 
};

#ifdef USE_EXTI
extern "C"
{

void EXTI0_IRQHandler(void) {
	exti._irqHandler(0);
}

void EXTI1_IRQHandler(void) {
	exti._irqHandler(1);
}

void EXTI2_IRQHandler(void) {
	exti._irq2Handler(2);
}

void EXTI3_IRQHandler(void) {
	exti._irq3Handler(3);
}

void EXTI4_IRQHandler(void) {
	exti._irq2Handler(4);
}

void EXTI5_9_IRQHandler(void) {
    for (uint8_t i = 5; i < 9; i++) {
        if (EXTI_GetFlagStatus(1 << i)) {
            exti._irqHandler(i);
        }
    }
}

void EXTI10_15_IRQHandler(void) {
    for (uint8_t i = 10; i < 15; i++) {
        if (EXTI_GetFlagStatus(1 << i)) {
            exti._irqHandler(i);
        }
    }
}

}
#endif

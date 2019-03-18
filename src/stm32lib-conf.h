#pragma once
#include "target.h"

#if defined(STM32F0)
#include "stm32f0xx_conf.h"
#elif defined(STM32F1)
#include "stm32f10x_conf.h"
#elif defined(STM32F3)
#include "stm32f30x_conf.h"
#else
#error
#endif

/* bit operations */
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

typedef struct it_callback {
  void (*callback)(void);
  it_callback *next = nullptr;
} it_callback_t;


#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "exti.h"
#include "flash.h"
#include "gpio.h"
#include "helpers.h"
#include "printing.h"
#include "scheduling.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"

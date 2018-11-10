#include "stm32lib-conf.h"

#define USART_BAUDRATE 1e6

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

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

#if defined(USE_USART_1)
void initUsart1(void)
{
#if defined(STM32F0)
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart1Rx.init(GPIO_Mode_AF);
    gpioUsart1Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
    gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
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

// Parameter configuration - retained across boots
// TODO we can squeeze this into 8 bit ints
// TODO add write count to avoid over-wearing flash
// The size of this struct MUST fall on 32bit boundaries
struct params_t {
	uint16_t boot_count;                     // boots since last mass erase

	uint16_t cell_v_min              = 3100; // minimum milliVolts per cell allowed
	uint16_t cell_v_min_timeout      = 2000; // milliseconds
	uint16_t cell_v_event_count      = 0;

	uint16_t temperature_max         = 5000; // centi-degrees C // TODO use kelvin to avoid negative numbers, or support negative numbers
	uint16_t temperature_max_timeout = 2000; // milliseconds
	uint16_t temperature_event_count = 0;

	uint16_t current_max             = 8000; // centiAmps 0~15000 80A default limit
	uint16_t current_max_timeout     = 2000; // milliseconds
	uint16_t current_event_count     = 0;

	uint16_t dynamic_reconfig        = 1;    // Allow dynamic reconfiguration via dip switch
	const uint16_t struct_size       = sizeof(params_t); // Size of this structure, for upgrading to subsequent versions of this structure // TODO version instead?

	uint32_t checksum                = 0;    // integrity check when loading from flash
} defaults;

struct params_t params;

Flash f {1, sizeof(params)/sizeof(uint16_t)};

void writeParams(void)
{
	params.checksum = crcCalcChecksum((uint32_t*)&params, sizeof(params)/sizeof(uint32_t) - 1);
	f.writeBlock((uint16_t*)&params, sizeof(params)/sizeof(uint16_t));
}

void initParameters(void)
{
    f.init();
    FLASH_SetLatency(FLASH_Latency_1);

    // TODO make sure this works for brand new device
    // Verify/load configuration from flash
    if (f.verifyChecksum()) {
    	f.readBlock((uint16_t*)&params, sizeof(params)/sizeof(uint16_t)); // load configuration
    } else {
    	//print("Bad Config!\r\n"); // use defaults
    	//f.printContents();
    }

    // Update boot count and immediately write back
    // We must immediately write to flash after every edit of params
    // We must update the checksum before committing to flash <- TODO move checksum step to flash library
    params.boot_count++;

    //writeParams(); // update boot count // TODO replace with param length? for endurance considerations and carryover after upgrade
}



// Introduce a corrupt block
// Useful for testing checksum and validating error handling
void writeCorruptedBlock(void)
{
	uint16_t badblock[3] = { 0xEFFE, 0xabcd, 0x2353 };
	f.writeBlock((uint16_t*)&badblock, 3);
}

// Print current parameter configuration
void printParams(void)
{
	print("\n\rbc: "); my_printInt(params.boot_count);

	print(" Vm: "); my_printInt(params.cell_v_min);
	print(" Vto: "); my_printInt(params.cell_v_min_timeout);
	print(" Ve: "); my_printInt(params.cell_v_event_count);

	print(" Tm: "); my_printInt(params.temperature_max);
	print(" Tto: "); my_printInt(params.temperature_max_timeout);
	print(" Te: "); my_printInt(params.temperature_event_count);

	print(" Cm: "); my_printInt(params.current_max);
	print(" Cto: "); my_printInt(params.current_max_timeout);
	print(" Ce: "); my_printInt(params.current_event_count);
}


int main()
{
    configureClocks(1000);

    gpioLed.init(GPIO_Mode_OUT);

    initUsart2();

    print("sup");
    mDelay(1000);
    // initParameters();
    // printParams();

    while (1) {
        mDelay(500);
        gpioLed.toggle();
    }

    return 0;
}

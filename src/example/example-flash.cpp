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
// TODO must be aligned on 4 byte boundary: make it 2 byte (like underlying flash hardware)
// TODO we can squeeze this into 8 bit ints
// TODO add write count to avoid over-wearing flash
// The size of this struct MUST fall on 32bit boundaries
struct params_t {
	uint16_t boot_count = 0;                     // boots since last mass erase
    uint16_t last_key = 0;
	uint32_t checksum = 0;    // integrity check when loading from flash
} defaults;

struct params_t params;

Flash f {1, sizeof(params)/sizeof(uint16_t)};

static uint8_t writeCount = 0;
void writeParams(void)
{
    if (writeCount > 30) {
        print("\n\rmax write count exceeded, not writing");
        return;
    }
    writeCount++;
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
    	print("Bad Config!\n\r"); // use defaults
    	f.printContents();
    }

    // Update boot count and immediately write back
    // We must immediately write to flash after every edit of params
    // We must update the checksum before committing to flash <- TODO move checksum step to flash library
    params.boot_count++;

    writeParams(); // update boot count // TODO replace with param length? for endurance considerations and carryover after upgrade
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
	print("\n\rboot count: "); my_printInt(params.boot_count);
	print(" last key pressed: "); _write(FD_STDOUT, (char*)&(params.last_key), 1);
}

int main()
{
    configureClocks(1000);

    gpioLed.init(GPIO_Mode_OUT);

    initUsart2();

    initParameters();
    printParams();

    while (1) {
        mDelay(500);
        gpioLed.toggle();

        // Read in characters, save the last pressed character
        int readchar = uart2.read();
        while(readchar != -1) {
            params.last_key = readchar;
            writeParams();
            printParams();
            readchar = uart2.read();
        }
    }

    return 0;
}

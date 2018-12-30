#define REG_FIFO_RX_BASE_ADDR 0x0F
#define REG_FIFO_TX_BASE_ADDR 0x0E
#define REG_FIFO_PTR_ADDR 0x0D
#define REG_PAYLOAD_LENGTH 0x22
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_FIFO_RX_NB_BYTES 0x13
#define REG_FIFO 0x0
#define REG_OP_MODE 0x01
#define REG_MODEM_CONFIG_1
#define REG_IRQ_FLAGS_MASK 0x11
#define REG_IRQ_FLAGS 0x12
#define REG_MODEM_STATUS 0x18
#define IRQ_MASK_RX_DONE 0x40
#define IRQ_MASK_TX_DONE 0x08
#define IRQ_MASK_RX_TIMEOUT 0x80

#define FIFO_TX_BASE_ADDR 0x80
#define FIFO_RX_BASE_ADDR 0x0

#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_OCP                  0x0b
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_SNR_VALUE        0x19
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_FREQ_ERROR_MSB       0x28
#define REG_FREQ_ERROR_MID       0x29
#define REG_FREQ_ERROR_LSB       0x2a
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_INVERTIQ             0x33
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_INVERTIQ2            0x3b
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42
#define REG_PA_DAC               0x4d

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// PA config
#define PA_BOOST                 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40

#define MAX_PKT_LENGTH           255

#include "spi.h"


class SX1276
{
    public:
    SX1276(Spi& spi, Gpio& nss, Gpio& nrst) : _spi(spi), _gpioNss(nss), _gpioNrst(nrst) {};
    Spi& _spi;
    Gpio& _gpioNss;
    Gpio& _gpioNrst;

    void writeRegister(uint8_t base, char data)
    {
        writeRegister(base, &data, 1);
    }

    void writeRegister(uint8_t base, char* data, uint16_t length)
    {
        static const uint8_t WRITE = 0x80;
        _gpioNss.reset();
        _spi.write(base | WRITE);
        _spi.write(data, length);
        _gpioNss.set();
    }

    char readRegister(uint8_t base, uint16_t length = 1)
    {
        _gpioNss.reset();
        _spi.read(base, length);
        _gpioNss.set();
        return _spi.rxBuf[0];
    }

void setMode(uint8_t mode)
{
    uint8_t m = 0x80;
    mode |= m;
    writeRegister(REG_OP_MODE, mode);
}

void dumpRegs()
{
    for (int i = 0; i < 128; i++) {
        uart1.write("0x",2);
        printHex(i);
        uart1.write(": 0x", 4);
        printHex(readRegister(i));
        printf("\r\n");
    }
}    

void init()
{
        setMode(0x0);
        setFrequency(900E6);

        // set lna boost
        writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
        // set auto AGC

        writeRegister(REG_MODEM_CONFIG_3, 0x04);

        writeRegister(REG_PA_DAC, 0x84);
        writeRegister(REG_OCP, 0x20 | (0x1F & 11));
        writeRegister(REG_PA_CONFIG, PA_BOOST | 15);


        writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);
        writeRegister(REG_MODEM_CONFIG_1, 0b10010010);
        writeRegister(REG_MODEM_CONFIG_2, 0x70);// spreading factor

        setMode(0x1);
        setMode(0x1);
    }

    void transmit(char* data, uint8_t length) {
        writeFIFO(data, length);
        tranny();
    }

    void tranny() {
        writeRegister(REG_IRQ_FLAGS, 0xFF); // todo reset tx only
        setMode(0b011); // todo enum
        bool txDone = false;
        while (!txDone)
        {
            readRegister(REG_OP_MODE);
            char flags = readRegister(REG_IRQ_FLAGS);
            txDone = flags & IRQ_MASK_TX_DONE;
        }
    }

    void transmit()
    {
        static uint16_t data = 1;
        writeFIFO((char*)&data, 16);
        data++;
        readRegister(REG_OP_MODE);
        readRegister(REG_IRQ_FLAGS);
        writeRegister(REG_IRQ_FLAGS, 0xFF);
        readRegister(REG_IRQ_FLAGS);
        setMode(0b011);
        readRegister(REG_OP_MODE);
        bool txDone = false;
        while (!txDone)
        {
            readRegister(REG_OP_MODE);
            char flags = readRegister(REG_IRQ_FLAGS);
            txDone = flags & IRQ_MASK_TX_DONE;
        }
        readRegister(REG_IRQ_FLAGS);
        readRegister(REG_IRQ_FLAGS);
        writeRegister(REG_IRQ_FLAGS, 0xFF);
    }
    void setFrequency(long frequency)
    {
        uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

        writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
        writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
        writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
    }

    uint8_t receive()
    {
        writeRegister(REG_IRQ_FLAGS, 0xFF);

        writeRegister(REG_FIFO_PTR_ADDR, REG_FIFO_RX_BASE_ADDR);

        setMode(0b110);

        bool rxDone = false;

        while (!rxDone)
        {
            char flags = readRegister(REG_IRQ_FLAGS);
            rxDone = flags & IRQ_MASK_RX_DONE;
            bool rxTimeout = flags & IRQ_MASK_RX_TIMEOUT;

            if (rxTimeout) {
                writeRegister(REG_IRQ_FLAGS, 0xFF);
                setMode(0b110);
            }
        }
        return readRegister(REG_FIFO_RX_NB_BYTES);
    }

    uint8_t* readFIFO(uint8_t length) {
        writeRegister(REG_FIFO_PTR_ADDR, 0);
        // writeRegister(REG_PAYLOAD_LENGTH, length);
        readRegister(REG_FIFO, length);
        return _spi.rxBuf;
    };

    void writeFIFO(char* data, uint8_t length)
    {
        // set FifoPtrAddr to FifoTxPtrBase
        writeRegister(REG_FIFO_PTR_ADDR, FIFO_TX_BASE_ADDR);
        writeRegister(REG_PAYLOAD_LENGTH, length);
        writeRegister(REG_FIFO, data, length);

        // write PayloadLength bytes to the FIFO
    };
};
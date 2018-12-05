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
#include "spi.h"
class SX1276
{
    public:
    SX1276(Spi& spi) : _spi(spi) {};
    Spi& _spi;


    

    void writeRegister(uint8_t base, char data)
    {
        writeRegister(base, &data, 1);
    }

    void writeRegister(uint8_t base, char* data, uint16_t length)
    {
        static const uint8_t WRITE = 0x80;
        _spi.init(SPI_BaudRatePrescaler_16);
        _spi.enable(ENABLE);
        _spi.write(base | WRITE);
        _spi.write(data, length);
        _spi.enable(DISABLE);
    }

    char readRegister(uint8_t base, uint16_t length = 1) {
        _spi.init(SPI_BaudRatePrescaler_16);
        _spi.enable(ENABLE);
        _spi.read(base, length);
        _spi.enable(DISABLE);
        return _spi.rxBuf[0];
    }

    void writeFIFO(char* data, uint8_t length)
    {
        // set FifoPtrAddr to FifoTxPtrBase
        writeRegister(REG_FIFO_PTR_ADDR, FIFO_TX_BASE_ADDR);
        writeRegister(REG_PAYLOAD_LENGTH, length);
        writeRegister(REG_FIFO, data, length);

        // write PayloadLength bytes to the FIFO
    };

    void setMode(uint8_t mode) {
        uint8_t m = 0x80;
        mode |= m;
        writeRegister(REG_OP_MODE, mode);
    }
    void init()
    {
        setMode(0x0);
        setMode(0x0);

        // readRegister(REG_IRQ_FLAGS_MASK);
        // writeRegister(REG_IRQ_FLAGS_MASK, ~IRQ_MASK_RX_DONE);
        // readRegister(REG_IRQ_FLAGS_MASK);

        readRegister(REG_IRQ_FLAGS);
        writeRegister(REG_IRQ_FLAGS, 0xFF);
        readRegister(REG_IRQ_FLAGS);

        setMode(0x1);
        setMode(0x1);
    }
    void transmit()
    {
        writeFIFO("asdf", 4);
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

    void receive()
    {
        mDelay(1);

        readRegister(REG_OP_MODE);
        setMode(0b110);
        readRegister(REG_OP_MODE);


        bool rxDone = false;

        while (!rxDone)
        {

            readRegister(REG_OP_MODE);
            char flags = readRegister(REG_IRQ_FLAGS);

            rxDone = flags & IRQ_MASK_RX_DONE;
             readRegister(REG_MODEM_STATUS);

        }

    }
    void readFIFO(uint8_t length);
};
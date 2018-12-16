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
    void dumpRegs()

{
      for (int i = 0; i < 128; i++) {
          uart1.write("0x",2);
    printHex(i);
    uart1.write(": 0x", 4);
    printHex(readRegister(i));
    printf("\r\n");
  }
}    void init()
    {
        setMode(0x0);
        setMode(0x0);
        setFrequency(433E6);
        // readRegister(REG_IRQ_FLAGS_MASK);
        // writeRegister(REG_IRQ_FLAGS_MASK, ~IRQ_MASK_RX_DONE);
        // readRegister(REG_IRQ_FLAGS_MASK);

        // set lna boost
    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
  // set auto AGC

  writeRegister(REG_MODEM_CONFIG_3, 0x04);

    // writeRegister(REG_PA_CONFIG, 0x70 | 14);

    writeRegister(REG_PA_DAC, 0x84);
    writeRegister(REG_OCP, 0x20 | (0x1F & 11));
    writeRegister(REG_PA_CONFIG, PA_BOOST | 15);
    
    // 
  writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);


//     uint8_t temp = readRegister(REG_DETECTION_OPTIMIZE) & 0xF8;
// if (true) {
//     writeRegister(REG_DETECTION_OPTIMIZE, (temp | 0x05)); // SF6
//     writeRegister(REG_DETECTION_THRESHOLD, 0x0C); // SF6
// } else {
//     writeRegister(REG_DETECTION_OPTIMIZE, (temp | 0x03)); // SF7-12
//     writeRegister(REG_DETECTION_THRESHOLD, 0x0A); // SF7-12  
// }
//         readRegister(REG_IRQ_FLAGS);
//         writeRegister(REG_IRQ_FLAGS, 0xFF);
//         readRegister(REG_IRQ_FLAGS);

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
    void setFrequency(long frequency)
{

  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

  writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
  writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
  writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
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
            readRegister(REG_MODEM_STATUS);
            char flags = readRegister(REG_IRQ_FLAGS);
            rxDone = flags & IRQ_MASK_RX_DONE;
            bool rxTimeout = flags & IRQ_MASK_RX_TIMEOUT;
            if (rxDone) {
                while(0);
            }
            if (rxTimeout) {
                mDelay(10);
                writeRegister(REG_IRQ_FLAGS, 0xFF);
                setMode(0b110);
            }

        }

    }
    void readFIFO(uint8_t length);
};
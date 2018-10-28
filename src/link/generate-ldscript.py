#!/usr/bin/env python

#stm32f0X0
productRamsizes = {
    "030": {
        "4": 4,
        "6": 4,
        "8": 8,
        "C": 32,
    },
    "070": {
        "6": 6,
        "B": 16,
    },
    #F0x2
    #https://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f0-series/stm32f0x2.html?querycriteria=productId=LN1823
    "072": {
        "8": 16,
        "B": 16,
    },
    #F103
    #https://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f1-series/stm32f103.html?querycriteria=productId=LN1565
    "103": {
        "4": 6,
        "6": 10,
        "8": 20,
        "B": 20,
        "C": 64,
        "D": 64,
        "E": 64,
        "F": 96,
        "G": 96,
    },
    "303": {
        "6": 16,
        "8": 16,
        "B": 40,
        "C": 40,
        "D": 40,
        "E": 40,
    }
}
# Stm32f051cX
#stm32f030f4p6
#4k ram 16kflash
#stm32f051 <pincount><flash size><package><temperaturerange>
productFlashsizes = {

        "4": 16,
        "6": 32,
        "8": 64,
        "B": 128,
        "C": 256,
        "D": 384,
        "E": 512,
        "F": 768,
        "G": 1024,
}

#STM32F103R8
def productFamily(product):
    return product[6:9]

def productFlash(product):
    return productFlashsizes[product[-1]]

def productRam(product):
    ramsizes = productRamsizes[productFamily(product)]
    return ramsizes[product[-1]]

def ldScript(ram, flash):

    estack = 0x20000000 | (int(ram) * 1024)
    return ("""
/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = %s;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0;      /* required amount of heap  */
_Min_Stack_Size = 0x800; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = %sK
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = %sK
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}""" % (hex(estack), flash, ram))

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument("-p", dest="product", required=True, help="STM32 product")
parser.add_argument("-o", dest="override", required=False, help="override flash size, k")
args = parser.parse_args()


product = args.product
flash = 0

if args.override:
    flash = args.override
else:
    flash = productFlash(product)

ram = productRam(product)

print(ldScript(ram, flash))

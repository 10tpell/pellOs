#ifndef __GPIO_H__
#define __GPIO_H__

#include "types.h"
#include "reg_utils.h"
#include "peripherals.h"

#define GPIO_MAX_PIN            53U

#define GPIO_BASE_PERIPHERAL    PERIPHERAL_BASE_ADDR + 0x200000U

#define GPIO_GPFSEL0_OFFSET     0x00U
#define GPIO_GPFSEL0_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL0_OFFSET
#define GPIO_GPFSEL1_OFFSET     0x04U
#define GPIO_GPFSEL1_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL1_OFFSET
#define GPIO_GPFSEL2_OFFSET     0x08U
#define GPIO_GPFSEL2_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL2_OFFSET
#define GPIO_GPFSEL3_OFFSET     0x0CU
#define GPIO_GPFSEL3_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL3_OFFSET
#define GPIO_GPFSEL4_OFFSET     0x10U
#define GPIO_GPFSEL4_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL4_OFFSET
#define GPIO_GPFSEL5_OFFSET     0x14U
#define GPIO_GPFSEL5_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPFSEL5_OFFSET

#define GPIO_GPSET0_OFFSET      0x1CU
#define GPIO_GPSET0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPSET0_OFFSET
#define GPIO_GPSET1_OFFSET      0x20U
#define GPIO_GPSET1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPSET1_OFFSET

#define GPIO_GPCLR0_OFFSET      0x28U
#define GPIO_GPCLR0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPCLR0_OFFSET
#define GPIO_GPCLR1_OFFSET      0x2CU
#define GPIO_GPCLR1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPCLR1_OFFSET

#define GPIO_GPLEV0_OFFSET      0x34U
#define GPIO_GPLEV0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPLEV0_OFFSET
#define GPIO_GPLEV1_OFFSET      0x38U
#define GPIO_GPLEV1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPLEV1_OFFSET

#define GPIO_GPEDS0_OFFSET      0x40U
#define GPIO_GPEDS0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPEDS0_OFFSET
#define GPIO_GPEDS1_OFFSET      0x44U
#define GPIO_GPEDS1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPEDS1_OFFSET

#define GPIO_GPREN0_OFFSET      0x4CU
#define GPIO_GPREN0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPREN0_OFFSET
#define GPIO_GPREN1_OFFSET      0x50U
#define GPIO_GPREN1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPREN1_OFFSET

#define GPIO_GPFEN0_OFFSET      0x58U
#define GPIO_GPFEN0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPFEN0_OFFSET
#define GPIO_GPFEN1_OFFSET      0x5CU
#define GPIO_GPFEN1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPFEN1_OFFSET

#define GPIO_GPHEN0_OFFSET      0x64U
#define GPIO_GPHEN0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPHEN0_OFFSET
#define GPIO_GPHEN1_OFFSET      0x68U
#define GPIO_GPHEN1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPHEN1_OFFSET

#define GPIO_GPLEN0_OFFSET      0x70U
#define GPIO_GPLEN0_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPLEN0_OFFSET
#define GPIO_GPLEN1_OFFSET      0x74U
#define GPIO_GPLEN1_ADDR        GPIO_BASE_PERIPHERAL + GPIO_GPLEN1_OFFSET

#define GPIO_GPAREN0_OFFSET     0x7CU
#define GPIO_GPAREN0_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPAREN0_OFFSET
#define GPIO_GPAREN1_OFFSET     0x08U
#define GPIO_GPAREN1_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPAREN1_OFFSET

#define GPIO_GPAFEN0_OFFSET     0x88U
#define GPIO_GPAFEN0_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPAFEN0_OFFSET
#define GPIO_GPAFEN1_OFFSET     0x8CU
#define GPIO_GPAFEN1_ADDR       GPIO_BASE_PERIPHERAL + GPIO_GPAFEN1_OFFSET

#define GPIO_PUP_PDN_CNTRL_REG0_OFFSET  0xE4U
#define GPIO_PUP_PDN_CNTRL_REG0_ADDR    GPIO_BASE_PERIPHERAL + GPIO_PUP_PDN_CNTRL_REG0_OFFSET
#define GPIO_PUP_PDN_CNTRL_REG1_OFFSET  0xE8U
#define GPIO_PUP_PDN_CNTRL_REG1_ADDR    GPIO_BASE_PERIPHERAL + GPIO_PUP_PDN_CNTRL_REG1_OFFSET
#define GPIO_PUP_PDN_CNTRL_REG2_OFFSET  0xECU
#define GPIO_PUP_PDN_CNTRL_REG2_ADDR    GPIO_BASE_PERIPHERAL + GPIO_PUP_PDN_CNTRL_REG2_OFFSET
#define GPIO_PUP_PDN_CNTRL_REG3_OFFSET  0xF0U
#define GPIO_PUP_PDN_CNTRL_REG3_ADDR    GPIO_BASE_PERIPHERAL + GPIO_PUP_PDN_CNTRL_REG3_OFFSET


#define GPIO_PUP_PDN_PULL_NONE          0x00U
#define GPIO_FUNC_INPUT                 0b000
#define GPIO_FUNC_OUTPUT                0b001
#define GPIO_FUNC_ALT_0                 0b100
#define GPIO_FUNC_ALT_1                 0b101
#define GPIO_FUNC_ALT_2                 0b110
#define GPIO_FUNC_ALT_3                 0b111
#define GPIO_FUNC_ALT_4                 0b011
#define GPIO_FUNC_ALT_5                 0b010

inline uint32_t gpio_writeReg(uint32_t* base, uint32_t pin, uint32_t value, uint8_t fieldSize) {
    if (GPIO_MAX_PIN < pin) return 0;

    uint32_t fieldMax = (1 << fieldSize) - 1; // mask of 1s for length of field (maximum value field in reg can be)
    if (value > fieldMax) return 0;

    uint8_t numFields = 32 / fieldSize;
    uint8_t shift = (pin % numFields) * fieldSize;
    uint32_t regAddr = base + (pin / numFields) * 4;

    uint32_t currVal = reg_read(regAddr);
    currVal &= ~(fieldMax << shift);
    currVal |= (value << shift);
    
    reg_write(regAddr, currVal);
    return 1;
}

inline uint32_t gpio_set(uint32_t pin) { return gpio_writeReg(GPIO_GPSET0_ADDR, pin, 1, 1); }
inline uint32_t gpio_clear(uint32_t pin) { return gpio_writeReg(GPIO_GPCLR0_ADDR, pin, 0, 1); }
inline uint32_t gpio_function(uint32_t pin, uint8_t func) { return gpio_writeReg(GPIO_GPFSEL0_ADDR, pin, func, 3); }
inline uint32_t gpio_pullUpDown(uint32_t pin, uint32_t val) { return gpio_writeReg(GPIO_PUP_PDN_CNTRL_REG0_ADDR, pin, val, 1); }

inline uint32_t gpio_useAlt(uint32_t pin, uint32_t alt) {
    if (gpio_pullUpDown(pin, GPIO_PUP_PDN_PULL_NONE)) {
        return gpio_function(pin, alt);
    }
    return 0;
}

inline uint32_t gpio_writePin(uint32_t pin, uint8_t value) { 
    if (value > 0) return gpio_set(pin);
    else return gpio_clear(pin);
}

#endif
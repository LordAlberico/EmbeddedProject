#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "miosix.h"
#include "print_util.h"
#include <functional>
#include "arch/common/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h"

using namespace miosix;

namespace Interrupt
{
    typedef Gpio<GPIOE_BASE, 0> PE0;
    typedef Gpio<GPIOA_BASE, 0> PA0;

    extern bool PA0_Interruption;

    void Interrupt_EXTI_PE0_init(void);

    void Interrupt_EXTI_PA0_init(void);
}

void __attribute__((naked)) EXTI0_IRQHandler();

#endif // INTERRUPT_H
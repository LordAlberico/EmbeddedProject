#include "Interrupt.h"

// #include "miosix.h"
// #include "print_util.h"
// #include <functional>
// #include "arch/common/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h"

using namespace miosix;

namespace Interrupt
{
    void Interrupt_EXTI_PE0_init(void)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;         // Enable the clock for GPIOE
        PE0::mode(Mode::INPUT);                      // Set PE0 to input mode
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;        // Enable the clock for the system configuration controller
        SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PE; // Set the EXTI0 to PE0
        EXTI->IMR |= EXTI_IMR_MR0;                   // Enable the interrupt mask for EXTI0
        EXTI->RTSR |= EXTI_RTSR_TR0;                 // Set the trigger to rising edge

        NVIC_SetPriority(EXTI0_IRQn, 15); // Set the priority of the interrupt
        NVIC_EnableIRQ(EXTI0_IRQn);       // Enable the interrupt
    }

    void Interrupt_EXTI_PA0_init(void)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        PA0::mode(Mode::INPUT);
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;
        EXTI->IMR |= EXTI_IMR_MR0;
        EXTI->RTSR |= EXTI_RTSR_TR0;

        NVIC_SetPriority(EXTI0_IRQn, 15);
        NVIC_EnableIRQ(EXTI0_IRQn);
    }
}

void __attribute__((naked)) EXTI0_IRQHandler()
{
    saveContext(); // Save the context
    if (EXTI->PR & EXTI_PR_PR0)
    {
        if (Interrupt::PE0::value())
        {
            exit(0);
        }
    }
    EXTI->PR = EXTI_PR_PR0;
    restoreContext(); // Restore the context
}
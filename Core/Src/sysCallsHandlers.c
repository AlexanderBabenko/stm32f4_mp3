/*
 * sysCalls.c
 *
 *  Created on: Nov 13, 2013
 *      Author: oleksandr.babenko
 */

#include "sysCallsHandlers.h"
#include "stm32f4xx.h"

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 exceptions initialization
 * \param  none
 * \return none
 */
int32_t SysCallsHandlersInit(void) {
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
    NVIC_SetPriority(UsageFault_IRQn, 0);

    return 0;
}

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 hard fault exception handler
 * \param  none
 * \return none
 */
void HardFault_Handler(void) {
    for (;;) {
        __asm volatile("nop");
    }
}

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 memory protection unit exception handler
 * \param  none
 * \return none
 */
void MemManage_Handler(void) {
    for (;;) {
        __asm volatile("nop");
    }
}

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 bus prefetch exception handler
 * \param  none
 * \return none
 */
void BusFault_Handler(void) {
    for (;;) {
        __asm volatile("nop");
    }
}

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 usage fault exception handler
 * \param  none
 * \return none
 */
void UsageFault_Handler(void) {
    // DIV By zero
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 9))) {
        for (;;) {
            __asm volatile("nop");
        }
    }
    // unaligned
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 8))) {
        for (;;) {
            __asm volatile("nop");
        }
    }
    // No coprocessor UsageFault
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 3))) {
        for (;;) {
            __asm volatile("nop");
        }
    }
    // Invalid PC load UsageFault
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 2))) {
        for (;;) {
            __asm volatile("nop");
        }
    }
    // Invalid state UsageFault
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 1))) {
        for (;;) {
            __asm volatile("nop");
        }
    }
    // Undefined instruction UsageFault
    if (SCB->CFSR & (1 << (SCB_CFSR_USGFAULTSR_Pos + 0))) {
        for (;;) {
            __asm volatile("nop");
        }
    }

    for (;;) {
        __asm volatile("nop");
    }
}

//------------------------------------------------------------------------------
/*
 * \brief  Cortex-M3 Debug monitor exception handler
 * \param  none
 * \return none
 */
void DebugMon_Handler(void) {
    for (;;) {
        __asm volatile("nop");
    }
}


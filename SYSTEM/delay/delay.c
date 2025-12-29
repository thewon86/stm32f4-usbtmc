#include "sys.h"
#include "delay.h"

#define OS_TICKS    1000U

void delay_ms(uint32_t Nms)
{
    uint32_t tick0, tick1;

    tick0 = tick1 = HAL_GetTick();
    while ((tick1 - tick0) < Nms) {
        tick1 = HAL_GetTick();
    }
}

void delay_us(uint32_t Nus)
{        
    uint32_t ticks;
    uint32_t told, tnow, tcnt=0;
    uint32_t reload;

    told = SysTick->VAL;
    reload = SysTick->LOAD;
    ticks = SystemCoreClock / 1000000UL * Nus;
    tcnt = 0;
    while (1) {
        tnow = SysTick->VAL;

        if (tnow > told) {
            tcnt += reload - tnow + told;
        } else {
            tcnt += told - tnow;
        }
        told = tnow;
        if (tcnt >= ticks) {
            break;
        }
    }
}

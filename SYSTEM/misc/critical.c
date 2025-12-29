#include <stdint.h>
#include "cmsis_armcc.h"

static int volatile critical_nesting_count;
static int volatile mask_state_first_critical_enter;

#define CLI() __disable_irq()
#define SEI() __enable_irq()

void critical_init(void)
{
    critical_nesting_count = 0;
}

void critical_enter(void)
{
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();
    critical_nesting_count ++;
    if(critical_nesting_count == 1)
    {
        mask_state_first_critical_enter = primask;
    }
}

void critical_exit(void)
{
    critical_nesting_count --;
    if(critical_nesting_count <= 0) {
        critical_nesting_count = 0;
        if (! mask_state_first_critical_enter) {
        }
    }
}

#ifndef __CRITICAL_H__
#define __CRITICAL_H__

#define ENTER_CRITICAL()    uint32_t primask = __get_PRIMASK(); __disable_irq();
#define EXIT_CRITICAL()     __set_PRIMASK(primask);

#endif

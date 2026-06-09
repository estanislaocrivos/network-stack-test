#ifndef MAIN_H
#define MAIN_H

#define ASSERT(cond)                   \
    do                                 \
    {                                  \
        if (!(cond))                   \
        {                              \
            __asm volatile("bkpt #0"); \
            while (1)                  \
            {                          \
            }                          \
        }                              \
    } while (0);

#include "stm32f4xx.h"

#endif /* MAIN_H */

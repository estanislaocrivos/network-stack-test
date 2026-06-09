#ifndef CLOCK_H
#define CLOCK_H

typedef enum
{
    CLOCK_16MHZ,
    CLOCK_48MHZ,
    CLOCK_84MHZ,
} clock_freq_t;

void clock_init(clock_freq_t freq);

#endif /* CLOCK_H */

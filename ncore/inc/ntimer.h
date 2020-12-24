#ifndef __NTIMER_H__
#define __NTIMER_H__

#include "config.h"

#define CLOCK_TICK_SEC  1000

struct ntimer_s {
        uint32_t interval;
        uint32_t start;
        uint32_t is_enable: 1;
        uint32_t padding: 31;
};

uint32_t ntimer_set(struct ntimer_s *timer, uint32_t interval);
void ntimer_reset(struct ntimer_s *timer);
uint8_t ntimer_expired(struct ntimer_s *timer);
uint32_t ntimer_clock(void);
void ntimer_tick(void);
void ntimer_start(struct ntimer_s *timer);
void ntimer_stop(struct ntimer_s *timer);

#endif /* __NTIMER_H__ */


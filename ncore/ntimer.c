#include "ntimer.h"

uint32_t time_clock = 0;

void ntimer_tick(void)
{
	time_clock++;
}

uint32_t ntimer_clock(void)
{
	return time_clock;
}

uint32_t ntimer_set(struct ntimer_s *timer, uint32_t interval)
{
	timer->interval = interval;
	timer->start = ntimer_clock();
	timer->is_enable = 1;
	return 0;
}

void ntimer_reset(struct ntimer_s *timer)
{
	timer->start = timer->start + timer->interval;
}

uint8_t ntimer_expired(struct ntimer_s *timer)
{
	if (!timer->is_enable) {
		return 0;
	}

	if ((int) (ntimer_clock() - timer->start) >= (int) timer->interval) {
		return 1;
	} else {
		return 0;
	}
}

void ntimer_start(struct ntimer_s *timer)
{
	timer->is_enable = 1;
}

void ntimer_stop(struct ntimer_s *timer)
{
	timer->is_enable = 0;
}


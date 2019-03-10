#include "Timer.h"



Timer::Timer() : start(clock())
{
}

Timer::Timer(clock_t s) : start(s)
{
}

double Timer::fps() const
{
	return 1.0 / getElapsed<units::secs>();

}

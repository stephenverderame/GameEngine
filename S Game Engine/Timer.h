#pragma once
#include <ctime>
enum class units {
	secs,
	milli,
	mins
};
class Timer
{
private:
	clock_t start;
public:
	Timer();
	Timer(clock_t s);
	void reset() { start = clock(); }

	template<units u = units::secs>
	double getElapsed() const {
		switch (u)
		{
		case units::secs:
			return static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
		case units::milli:
			return clock() - start;
		default: //mins
			return (static_cast<double>(clock() - start) / CLOCKS_PER_SEC) / 60.0;
		}
	}
	double fps() const;
};


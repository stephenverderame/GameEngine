#include "Observer.h"

void Subject::notify(int msg, void * data)
{
	for (auto it : observers) {
		it->update(msg, data);
	}
}

void Subject::attatch(Observer * o)
{
	observers.push_back(o);
}


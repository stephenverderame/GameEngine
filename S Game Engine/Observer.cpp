#include "Observer.h"

void Subject::notify(int msg, void * data)
{
	observer->update(msg, data);
}

void Subject::attatch(Observer * o)
{
	observer = o;
}

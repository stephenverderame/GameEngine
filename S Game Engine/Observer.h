#pragma once
#include <vector>
class Observer
{
public:
	virtual void update(int msg, void * data) = 0;
};
class Subject {
protected:
	std::vector<Observer *> observers;
public:
	void notify(int msg, void * data);
	void attatch(Observer * o);
};


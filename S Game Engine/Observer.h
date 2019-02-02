#pragma once
class Observer
{
public:
	virtual void update(int msg, void * data) = 0;
};
class Subject {
protected:
	Observer * observer;
public:
	void notify(int msg, void * data);
	void attatch(Observer * o);
};


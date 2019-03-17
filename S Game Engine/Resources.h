#pragma once
#include <memory>
#include <stdResources\resource.h>
struct w_imp;
class Resources
{
public:
	char * data;
	int length;
private:
	void load(int resourceId, int resourceType);
public:
	Resources(int resourceId, int resourceType);
	Resources(char * data, int length) : data(data), length(length) {};
	Resources() : data(nullptr), length(0) {};
	Resources& operator=(const Resources & other);
	~Resources();
};
class StdResources {
private:
	std::unique_ptr<w_imp> pimpl;
	static bool init;
public:
	StdResources();
	~StdResources();
	std::unique_ptr<Resources> loadStdResource(int resourceId, int resourceType);
};



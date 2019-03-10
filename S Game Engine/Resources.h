#pragma once
#define SHADER 5787
#define COMMON_VERT 10
#define COMMON_FRAG 11
#define MODEL_VERT 12
#define MODEL_FRAG 13
#include <memory>
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



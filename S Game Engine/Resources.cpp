#include "Resources.h"
#define NOMINMAX
#include <Windows.h>
bool StdResources::init = false;
struct w_imp {
	HMODULE moduleHandle;
};
void Resources::load(int resourceId, int resourceType)
{
	HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId), MAKEINTRESOURCE(resourceType));
	if (hRes) {
		HGLOBAL hLoad = LoadResource(GetModuleHandle(NULL), hRes);
		if (hLoad) {
			LPVOID res = LockResource(hLoad);
			if (res) {
				length = SizeofResource(GetModuleHandle(NULL), hRes);
				if (data != nullptr) delete[] data;
				data = new char[length+1];
				memcpy(data, (char*)res, length);
				data[length] = '\0';

			}
		}
	}
}

Resources::Resources(int resourceId, int resourceType) : Resources()
{
	load(resourceId, resourceType);
}


Resources & Resources::operator=(const Resources & other)
{
	if (other.data != nullptr) {
		if (data != nullptr) delete[] data;
		data = new char[other.length + 1];
		length = other.length;
		memcpy(data, other.data, length);
		data[length] = '\0';
	}
	else {
		if (data != nullptr) delete[] data;
		data = nullptr;
		length = 0;
	}
	return *this;
}

Resources::~Resources()
{
	if(data != nullptr)
		delete[] data;
}

StdResources::StdResources()
{
	if (!StdResources::init) {
		pimpl = std::make_unique<w_imp>();
		pimpl->moduleHandle = LoadLibraryEx("stdResources.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
		StdResources::init = true;
	}
}

StdResources::~StdResources()
{
	StdResources::init = false;
	FreeLibrary(pimpl->moduleHandle);
}

std::unique_ptr<Resources> StdResources::loadStdResource(int resourceId, int resourceType)
{
	HRSRC hRes = FindResource(pimpl->moduleHandle, MAKEINTRESOURCE(resourceId), MAKEINTRESOURCE(resourceType));
	if (hRes) {
		HGLOBAL hLoad = LoadResource(pimpl->moduleHandle, hRes);
		if (hLoad) {
			LPVOID res = LockResource(hLoad);
			if (res) {
				int length = SizeofResource(pimpl->moduleHandle, hRes);
				char * data = new char[length + 1];
				memcpy(data, (char*)res, length);
				data[length] = '\0';
				return std::make_unique<Resources>(data, length);

			}
		}
	}
	return std::make_unique<Resources>();
}

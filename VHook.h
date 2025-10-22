#pragma once

#define VHOOK_USE_EXCEPTIONS	1
#define VHOOK_USE_ISBADREADPTR	1
#define VHOOK_MALLOC			malloc
#define VHOOK_FREE				free
#define VHOOK_MEMCPY			memcpy

class VHook
{
public:
	VHook() = delete;
	VHook(const VHook&) = delete;
	VHook(void* Object);
	template<typename T>
	VHook(T* Object) : VHook(reinterpret_cast<void*>(Object)) {}
	~VHook();

	void* Add(const unsigned int Index, void* Detour);
	void Remove(const unsigned int Index);

protected:
	bool IsIndexHooked(const unsigned int Index);

	void* Object = nullptr;
	void** OriginalVTable = nullptr;
	void** NewVTable = nullptr;
	size_t NumFuncs = 0;
};


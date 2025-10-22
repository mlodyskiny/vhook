#include "VHook.h"

#if VHOOK_USE_EXCEPTIONS
#include <stdexcept>
#define VHOOK_EXCEPT(Message) throw std::runtime_error(Message)
#else
#define VHOOK_EXCEPT(Message) *reinterpret_cast<char**>(0xDEADBEEF) = const_cast<char*>(Message)
#endif

#if VHOOK_USE_ISBADREADPTR
#include <Windows.h>
#endif

VHook::VHook(void* Object) : Object(Object)
{
	if (!Object)
	{
		VHOOK_EXCEPT("invalid object ptr");
	}
#if VHOOK_USE_ISBADREADPTR
	if (IsBadReadPtr(Object, sizeof(void*)))
	{
		VHOOK_EXCEPT("cant read object ptr to get the vtable");
	}
#endif

	OriginalVTable = *reinterpret_cast<void***>(Object);
	if (!OriginalVTable)
	{
		VHOOK_EXCEPT("objects vtable is nullptr");
	}
#if VHOOK_USE_ISBADREADPTR
	if (IsBadReadPtr(OriginalVTable, sizeof(void*)))
	{
		VHOOK_EXCEPT("cant read objects vtable");
	}
#endif

	while (OriginalVTable[NumFuncs])
	{
		++NumFuncs;
	}

	NewVTable = reinterpret_cast<void**>(VHOOK_MALLOC(NumFuncs * sizeof(void*)));
	if (!NewVTable)
	{
		VHOOK_EXCEPT("failed to allocate memory for new vtable");
	}
	VHOOK_MEMCPY(NewVTable, OriginalVTable, NumFuncs * sizeof(void*));
	
	*reinterpret_cast<void***>(Object) = NewVTable;
}

VHook::~VHook()
{
	if (Object && OriginalVTable) 
	{
	#if VHOOK_USE_ISBADREADPTR
		if (!IsBadReadPtr(Object, sizeof(void*)))
	#endif
		{
			*reinterpret_cast<void***>(Object) = OriginalVTable;
		}
	}
	
	if (NewVTable)
	{
		free(NewVTable);
	}
}

void* VHook::Add(const unsigned int Index, void* Detour)
{
	if (Index >= NumFuncs)
	{
		VHOOK_EXCEPT("index higher than number of funcs in vtable");
	}

	if (IsIndexHooked(Index))
	{
		VHOOK_EXCEPT("function already hooked");
	}

	NewVTable[Index] = Detour;
	return OriginalVTable[Index];
}

void VHook::Remove(const unsigned int Index)
{
	if (Index >= NumFuncs)
	{
		VHOOK_EXCEPT("index higher than number of funcs in vtable");
	}

	if (!IsIndexHooked(Index))
	{
		VHOOK_EXCEPT("function wasnt hooked");
	}

	NewVTable[Index] = OriginalVTable[Index];
}

bool VHook::IsIndexHooked(const unsigned int Index)
{
	if (Index >= NumFuncs)
	{
		VHOOK_EXCEPT("index higher than number of funcs in vtable");
	}

	return OriginalVTable[Index] != NewVTable[Index];
}


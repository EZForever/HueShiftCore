// You can create hook instances by using HOOK_INSTANCE_* macros from hook.h
#include "hook.h"

// If debug logging is enabled (HUESHIFT_ALLOC_CONSOLE defined in config.h), LOG() and PAUSE() macros are available from logging.h
#include "logging.h"

// To hook an API (or any prototyped function that can be linked at compile time), use HOOK_INSTANCE()
HOOK_INSTANCE(GetSystemTime)
{
	// Then implement a function with idential prototype under the name "HandlerRoutine". This will be the hook callback function
	// If you're trying to hook a Windows API, it is recommended to copy the prototype directly from SDK headers
	// And don't forget about calling convention! For Windows APIs it should be WINAPI
	VOID WINAPI HandlerRoutine(LPSYSTEMTIME lpSystemTime)
	{
		// You can call the original (unhooked) function from within HandlerRoutine by OriginalRoutine()
		// It is automatically defined to have the same prototype so you can pass any parameters directly
		OriginalRoutine(lpSystemTime);

		// Note that LOG() uses Unicode strings
		LOG(L"[I] GetSystemTime(): Current UTC time is %02u:%02u\n", lpSystemTime->wHour, lpSystemTime->wMinute);
	}
}

// Sometimes the function you wish to hook is not available for compile-time linking, e.g. functions from another version of CRT
// HOOK_INSTANCE_EX() allows you to separate function prototype from implementation
// For example this hook instance applies to vcruntime140!memcpy() (resolved at run-time), but uses prototype from current CRT
HOOK_INSTANCE_EX(memcpy, "vcruntime140", "memcpy")
{
	void* __cdecl HandlerRoutine(void* dest, void const* src, size_t count)
	{
		return OriginalRoutine(dest, src, count);
	}
}

#if 0

// For hooking a private (not exported) function, you would need HOOK_INSTANCE_OFFSET()
// Prototype of the function is still required, so you may need to define it manually

BOOL WINAPI MyTargetFunction(BOOL fEnable);

HOOK_INSTANCE_OFFSET(MyTargetFunction, "MyTargetLibrary.dll", 0x1234)
{
	BOOL WINAPI HandlerRoutine(BOOL fEnable)
	{
		return OriginalRoutine(fEnable);
	}
}

#endif


#include "hook.h"

#include "logging.h"

#include <oleauto.h>

// IAT hook instances work similarly as Detours ones (see hook.cpp in CoreExample), but with two major differences:
// 1. They use IAT_HOOK_INSTANCE_* macros instead
// 2. You always need to specify the module name (or, more percisely, the name used in IAT)
// That second point is important since IAT hook instances don't do any detection on import redirection shenanigans

// Here's an example of IAT hooking GetSystemTime() called from cmd.exe
// GetSystemTime() is said to be in KERNEL32.DLL in MSDN docs, but actual implementation on current Windows versions is in KERNELBASE.DLL
// However, cmd.exe is compiled with ApiSets support, thus the module name used in its IAT is "api-ms-win-core-sysinfo-l1-1-0.dll"
// So that's what we put here. Moral of the story: always double-check with the IAT to avoid ApiSets or ".dll" suffix problems
IAT_HOOK_INSTANCE("api-ms-win-core-sysinfo-l1-1-0.dll", GetSystemTime)
{
	VOID WINAPI HandlerRoutine(LPSYSTEMTIME lpSystemTime)
	{
		// OriginalRoutine is still there and functions as normal
		// However you can just call the original routine by its name since it is left untouched anyway
		OriginalRoutine(lpSystemTime);

		LOG(L"[I] GetSystemTime(): Current UTC time is %02u:%02u\n", lpSystemTime->wHour, lpSystemTime->wMinute);
	}
}

// IAT hooks can only affect a single IAT, i.e. a single module
// To hook modules other than the main EXE, use IAT_HOOK_INSTANCE_EX() and specify target module name as the *first* argument
// This name is passed to GetModuleHandle() on load, so not as strict as the one above
IAT_HOOK_INSTANCE_EX("UCRTBASE", "api-ms-win-core-sysinfo-l1-1-0.dll", GetLocalTime)
{
	VOID WINAPI HandlerRoutine(LPSYSTEMTIME lpSystemTime)
	{
		OriginalRoutine(lpSystemTime);

		LOG(L"[I] GetLocalTime(): Current local time is %02u:%02u\n", lpSystemTime->wHour, lpSystemTime->wMinute);
	}
}

#if 0

// If your target function is *imported* by ordinal by the target module, specifying the resolved routine name will not work
// Use IAT_HOOK_INSTANCE_ORDINAL() / IAT_HOOK_INSTANCE_ORDINAL_EX() instead
IAT_HOOK_INSTANCE_ORDINAL("OLEAUT32.DLL", LoadTypeLib, 161)
{
	HRESULT STDAPICALLTYPE HandlerRoutine(LPCOLESTR szFile, ITypeLib** pptlib)
	{
		LOG(L"[I] LoadTypeLib(): Loading TypeLib %s\n", szFile);

		return OriginalRoutine(szFile, pptlib);
	}
}

// Despite the macro's name, it also supports specifying a string as the "ordinal"
// Useful for mangled export names or routines with custom prototypes

BOOL WINAPI MyTargetFunction(BOOL fEnable);

IAT_HOOK_INSTANCE_ORDINAL_EX("MyTarget.dll", "MyModule.dll", MyTargetFunction, "MyExport")
{
	BOOL WINAPI HandlerRoutine(BOOL fEnable)
	{
		return OriginalRoutine(fEnable);
	}
}

#endif


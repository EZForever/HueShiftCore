#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <detours.h>

bool __HookImplRegister(const char* szName, PVOID* ppvOriginal, const PVOID pvHandler);

#define __HOOK_INSTANCE_BASE(proto, target, name) \
	namespace __HookInstance_##proto \
	{ \
		typedef decltype(proto) ROUTINE, *PROUTINE; \
		PROUTINE OriginalRoutine = target; \
		ROUTINE HandlerRoutine; \
		\
		auto __init = __HookImplRegister(name, (PVOID*)&OriginalRoutine, HandlerRoutine); \
	} \
	\
	namespace __HookInstance_##proto // { ... }

// HOOK_INSTANCE(WriteConsole) { ... }
#define HOOK_INSTANCE(target) \
	__HOOK_INSTANCE_BASE(target, target, #target)

/*
// HOOK_INSTANCE_EX(memcpy, "vcruntime140", "memcpy") { ... }
#define HOOK_INSTANCE_EX(proto, library, target) \
	__HOOK_INSTANCE_BASE(proto, (PROUTINE)GetProcAddress(GetModuleHandleA(library), target), library "!" target)
*/
// HOOK_INSTANCE_EX(memcpy, "vcruntime140", "memcpy") { ... }
#define HOOK_INSTANCE_EX(proto, library, target) \
	__HOOK_INSTANCE_BASE(proto, (PROUTINE)DetourFindFunction(library, target), library "!" target)

// HOOK_INSTANCE_OFFSET(memcpy, "vcruntime140", 0x1234) { ... }
#define HOOK_INSTANCE_OFFSET(proto, library, offset) \
	__HOOK_INSTANCE_BASE(proto, (PROUTINE)((PBYTE)GetModuleHandleA(library) + offset), library "+" #offset)


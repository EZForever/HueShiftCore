#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

bool __HookImplRegister(LPCSTR szName, LPCSTR szTarget, LPCSTR szModule, LPCSTR szProc, PVOID* ppvOriginal, const PVOID pvHandler);

#define __IAT_HOOK_INSTANCE_BASE_(ctr, proto, target, mod, proc) \
	namespace __IatHookInstance_##ctr##_##proto \
	{ \
		typedef decltype(proto) ROUTINE, *PROUTINE; \
		PROUTINE OriginalRoutine; \
		ROUTINE HandlerRoutine; \
		\
		auto __init = __HookImplRegister( \
			mod "!" #proto, \
			target, \
			mod, \
			((UINT_PTR)proc >= MAXUINT16) ? (LPCSTR)proc : (LPCSTR)(IMAGE_ORDINAL_FLAG | (WORD)(UINT_PTR)proc), \
			(PVOID*)&OriginalRoutine, \
			HandlerRoutine \
		); \
	} \
	\
	namespace __IatHookInstance_##ctr##_##proto // { ... }

#define __IAT_HOOK_INSTANCE_BASE(ctr, proto, target, mod, proc) \
	__IAT_HOOK_INSTANCE_BASE_(ctr, proto, target, mod, proc)

// IAT_HOOK_INSTANCE("KERNEL32.DLL", WriteConsole) { ... }
#define IAT_HOOK_INSTANCE(mod, proc) \
	__IAT_HOOK_INSTANCE_BASE(__COUNTER__, proc, NULL, mod, #proc)

// IAT_HOOK_INSTANCE_EX("MyLibrary.dll", "KERNEL32.DLL", WriteConsole) { ... }
#define IAT_HOOK_INSTANCE_EX(target, mod, proc) \
	__IAT_HOOK_INSTANCE_BASE(__COUNTER__, proc, target, mod, #proc)

// IAT_HOOK_INSTANCE_ORDINAL("OLEAUT32.DLL", LoadTypeLib, 161) { ... }
#define IAT_HOOK_INSTANCE_ORDINAL(mod, proto, ord) \
	__IAT_HOOK_INSTANCE_BASE(__COUNTER__, proto, NULL, mod, ord)

// IAT_HOOK_INSTANCE_ORDINAL_EX("MyLibrary.dll", "KERNEL32.DLL", WriteConsole) { ... }
#define IAT_HOOK_INSTANCE_ORDINAL_EX(target, mod, proto, ord) \
	__IAT_HOOK_INSTANCE_BASE(__COUNTER__, proto, target, mod, ord)


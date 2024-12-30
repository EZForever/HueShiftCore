#include "hook.h"

decltype(g_hModule) g_hModule;
decltype(g_hookInstances) g_hookInstances;

bool __HookInitialize(const char* name, PVOID* pOriginal, const PVOID handler)
{
	if (*pOriginal)
	{
		PBYTE pbCode = (PBYTE)*pOriginal;
		PBYTE pbCodeLast = NULL;
		while (pbCode != pbCodeLast)
		{
			pbCodeLast = pbCode;

#if defined(DETOURS_X86) || defined(DETOURS_X64)
			// This is for dealing with import stubs introduced by Debug builds
			// jmp rel32
			if (pbCode[0] == 0xE9)
			{
				pbCode = pbCode + 5 + *(UNALIGNED INT32*)&pbCode[1];
			}
#endif

#if defined(DETOURS_X64)
			// This is for dealing with ApiSet import aliases 
			// rex.w jmp qword [rip + rel32]
			if (pbCode[0] == 0x48 && pbCode[1] == 0xFF && pbCode[2] == 0x25)
			{
				pbCode = *(UNALIGNED PBYTE*)(pbCode + 7 + *(UNALIGNED INT32*)&pbCode[3]);
			}
#endif

			// Process all other cases with Detours
			pbCode = (PBYTE)DetourCodeFromPointer(pbCode, NULL);
		}

		*pOriginal = pbCode;
	}
	g_hookInstances.push_back(std::make_tuple(name, pOriginal, handler));
	return true;
}


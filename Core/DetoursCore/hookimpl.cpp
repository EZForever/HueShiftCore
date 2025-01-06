#include "hookimpl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tuple>
#include <vector>

#include <detours.h>

#include "logging.h"

#pragma warning(disable: 4073) // Use of library initialization area
#pragma init_seg(lib)

// szName, ppvOriginal, pvHandler
static std::vector<std::tuple<const char*, PVOID*, const PVOID>> g_hookInstances;

void HookImplInit()
{
	// Restore IAT if needed
	DetourRestoreAfterWith();
}

void HookImplInstall()
{
	DetourTransactionBegin();
	for (const auto& hook : g_hookInstances)
	{
		auto szName = std::get<0>(hook);
		auto ppvOriginal = std::get<1>(hook);
		auto pvHandler = std::get<2>(hook);
		if (!*ppvOriginal)
		{
			LOG(L"[W] Hook routine %S could not be found, ignoring.\n", szName);
			continue;
		}

		LOG(L"[I] Hook %S = %p -> %p ...\n", szName, *ppvOriginal, pvHandler);
		DetourAttach(ppvOriginal, pvHandler);
	}
	DetourTransactionCommit();
}

void HookImplUninstall()
{
	DetourTransactionBegin();
	for (const auto& hook : g_hookInstances)
	{
		auto szName = std::get<0>(hook);
		auto ppvOriginal = std::get<1>(hook);
		auto pvHandler = std::get<2>(hook);
		if (!*ppvOriginal)
			continue;

		LOG(L"[I] Unhook %S...\n", szName);
		DetourDetach(ppvOriginal, pvHandler);
	}
	DetourTransactionCommit();
}

bool __HookImplRegister(const char* szName, PVOID* ppvOriginal, const PVOID pvHandler)
{
	if (*ppvOriginal)
	{
		PBYTE pbCode = (PBYTE)*ppvOriginal;
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

		*ppvOriginal = pbCode;
	}
	g_hookInstances.emplace_back(szName, ppvOriginal, pvHandler);
	return true;
}


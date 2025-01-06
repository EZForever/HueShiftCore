#include "hookimpl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

#include <tuple>
#include <vector>

#include "logging.h"

#pragma comment(lib, "dbghelp.lib")

#pragma warning(disable: 4073) // Use of library initialization area
#pragma init_seg(lib)

// szName, ppvEntry, pvOriginal, pvHandler
static std::vector<std::tuple<const char*, PVOID*, const PVOID, const PVOID>> g_hookInstances;
static LONG g_lIatLock = 0;

static PVOID* IatGetEntry(HMODULE hTarget, LPCSTR szModule, LPCSTR szProc)
{
    ULONG cbImportDir;
    auto pImportDir = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToDataEx(hTarget, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &cbImportDir, NULL);
    if (!pImportDir)
    {
        return NULL;
    }

    PIMAGE_THUNK_DATA pLookupTable = NULL;
    PIMAGE_THUNK_DATA pAddressTable = NULL;
    for (int i = 0; i < cbImportDir / sizeof(IMAGE_IMPORT_DESCRIPTOR); i++)
    {
        auto& entry = pImportDir[i];
        if (!entry.Name || !entry.OriginalFirstThunk || !entry.FirstThunk)
        {
            continue;
        }

        auto pName = (const char*)((PBYTE)hTarget + entry.Name);
        if (pName && !_stricmp(pName, szModule))
        {
            pLookupTable = (PIMAGE_THUNK_DATA)((PBYTE)hTarget + entry.OriginalFirstThunk);
            pAddressTable = (PIMAGE_THUNK_DATA)((PBYTE)hTarget + entry.FirstThunk);
            break;
        }
    }
    if (!pLookupTable || !pAddressTable)
    {
        return NULL;
    }

    WORD wProcOrdinal = IMAGE_SNAP_BY_ORDINAL((UINT_PTR)szProc) ? IMAGE_ORDINAL((UINT_PTR)szProc) : 0;
    while (pLookupTable->u1.Ordinal)
    {
        WORD wOrdinal = IMAGE_SNAP_BY_ORDINAL(pLookupTable->u1.Ordinal) ? IMAGE_ORDINAL(pLookupTable->u1.Ordinal) : 0;
        if (wOrdinal == wProcOrdinal)
        {
            auto pNameTable = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hTarget + pLookupTable->u1.AddressOfData);
            if (wOrdinal != 0 || !strcmp(pNameTable->Name, szProc))
            {
                return (void**)&pAddressTable->u1.Function;
            }
        }

        pLookupTable++;
        pAddressTable++;
    }

    return NULL;
}

static BOOL IatModifyEntry(PVOID* ppvEntry, const PVOID pvValue)
{
    // Yes, this is a spinlock; modifying an IAT entry shouldn't take that long though
    while (InterlockedCompareExchange(&g_lIatLock, -1, 0) != 0)
    {
        YieldProcessor();
    }

    DWORD flOldProtect;
    BOOL fRet = VirtualProtect(ppvEntry, sizeof(*ppvEntry), PAGE_READWRITE, &flOldProtect);
    if (fRet)
    {
        InterlockedExchangePointer(ppvEntry, pvValue);
        VirtualProtect(ppvEntry, sizeof(*ppvEntry), flOldProtect, &flOldProtect);
    }

    InterlockedAnd(&g_lIatLock, 0);
    return fRet;
}

void HookImplInit()
{
	// Nothing to do
}

void HookImplInstall()
{
    for (const auto& hook : g_hookInstances)
    {
        auto szName = std::get<0>(hook);
        auto ppvEntry = std::get<1>(hook);
        auto pvOriginal = std::get<2>(hook);
        auto pvHandler = std::get<3>(hook);
        if (!ppvEntry || !pvOriginal)
        {
            LOG(L"[W] Hook routine %S could not be found, ignoring.\n", szName);
            continue;
        }

        LOG(L"[I] Hook %S @ %p = %p -> %p ...\n", szName, ppvEntry, pvOriginal, pvHandler);
        IatModifyEntry(ppvEntry, pvHandler);
    }
}

void HookImplUninstall()
{
    for (const auto& hook : g_hookInstances)
    {
        auto szName = std::get<0>(hook);
        auto ppvEntry = std::get<1>(hook);
        auto pvOriginal = std::get<2>(hook);
        auto pvHandler = std::get<3>(hook);
        if (!pvOriginal)
            continue;

        LOG(L"[I] Unhook %S @ %p...\n", szName, ppvEntry);
        IatModifyEntry(ppvEntry, pvOriginal);
    }
}

bool __HookImplRegister(LPCSTR szName, LPCSTR szTarget, LPCSTR szModule, LPCSTR szProc, PVOID* ppvOriginal, const PVOID pvHandler)
{
	auto hTarget = GetModuleHandleA(szTarget);
    auto ppvEntry = hTarget ? IatGetEntry(hTarget, szModule, szProc) : NULL;
    *ppvOriginal = ppvEntry ? *ppvEntry : NULL;
    g_hookInstances.emplace_back(szName, ppvEntry, *ppvOriginal, pvHandler);
	return true;
}


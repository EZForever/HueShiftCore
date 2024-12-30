#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tuple>
#include <cstdio>

#include <detours.h>

#include "hook.h"
#include "logging.h"

#include "config.h"

BOOL DllAttach(HMODULE hModule)
{
    g_hModule = hModule;

    // Restore IAT if needed
    DetourRestoreAfterWith();

#ifdef HUESHIFT_ALLOC_CONSOLE
    // Allocate console (for debug output) if none exists
    if (!GetConsoleWindow() && AllocConsole())
    {
        // If the console is allocated by us, minimize it immediately to reduce clutter
        ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

        // Re-open stdio handles, so we can use printf(), puts(), etc
        FILE* _pFile;
        freopen_s(&_pFile, "CONIN$", "r+t", stdin);
        freopen_s(&_pFile, "CONOUT$", "w+t", stdout);
        freopen_s(&_pFile, "CONOUT$", "w+t", stderr);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
#endif

    // Display some welcome message
    LOG(L"libHueShift v0.0.1 Indev (" __DATE__ ", Build tag \"" HUESHIFT_BUILD_TAG "\")\n\n");

    DetourTransactionBegin();
    for (const auto& hook : g_hookInstances)
    {
        auto name = std::get<0>(hook);
        auto pOriginal = std::get<1>(hook);
        auto handler = std::get<2>(hook);
        if (!*pOriginal)
        {
            LOG(L"[W] Hook routine %S could not be found, ignoring.\n", name);
            continue;
        }

        LOG(L"[I] Hook %S = %p -> %p ...\n", name, *pOriginal, handler);
        DetourAttach(pOriginal, handler);
    }
    DetourTransactionCommit();
    LOG(L"[I] Routines hooked\n");

    return TRUE;
}

BOOL DllDetach(HMODULE hModule)
{
    // Unhook critical functions
    DetourTransactionBegin();
    for (const auto& hook : g_hookInstances)
    {
        auto name = std::get<0>(hook);
        auto pOriginal = std::get<1>(hook);
        auto handler = std::get<2>(hook);
        if (!*pOriginal)
            continue;

        LOG(L"[I] Unhook %S...\n", name);
        DetourDetach(pOriginal, handler);
    }
    DetourTransactionCommit();
    LOG(L"[I] Routines unhooked\n");

#ifdef HUESHIFT_ALLOC_CONSOLE
    // Free the allocated console
    FreeConsole();
#endif

    g_hModule = NULL;
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return DllAttach(hModule);
    case DLL_PROCESS_DETACH:
        return DllDetach(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

#ifdef HUESHIFT_DUMMY_EXPORT

extern "C"
__declspec(dllexport)
DWORD DllDummyExport()
{
    return 0x91779111;
}

#endif


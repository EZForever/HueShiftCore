#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <detours.h>

#include "config.h"

#pragma comment(lib, "shlwapi.lib")

WCHAR corePath[MAX_PATH];
WCHAR victimPath[MAX_PATH];
WCHAR* victimArgs;

BOOL SetPrivilege(HANDLE hProcess, LPCWSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    HANDLE hToken;
    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
        return FALSE;

    LUID luid;
    if (!LookupPrivilegeValueW(NULL, lpszPrivilege, &luid))
    {
        CloseHandle(hToken);
        return FALSE;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        CloseHandle(hToken);
        return FALSE;
    }

    CloseHandle(hToken);
    return TRUE;
}

#ifdef HUESHIFT_LAUNCH_WITH_DETOURS

bool LaunchVictim(DWORD dwCreationFlags, PROCESS_INFORMATION& pi)
{
    size_t dllPathSize;
    char corePathA[MAX_PATH] = { 0 };
    wcstombs_s(&dllPathSize, corePathA, corePath, MAX_PATH - 1);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    if (!DetourCreateProcessWithDllExW(victimPath, victimArgs, NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi, corePathA, NULL))
    {
        fwprintf(stderr, L"[F] DetourCreateProcessWithDllEx() failed, GetLastError() = %d\n", GetLastError());
        return false;
    }
    return true;
}

#else

bool LaunchVictim(DWORD dwCreationFlags, PROCESS_INFORMATION& pi)
{
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    if (!CreateProcessW(victimPath, victimArgs, NULL, NULL, FALSE, dwCreationFlags | CREATE_SUSPENDED, NULL, NULL, &si, &pi))
    {
        fwprintf(stderr, L"[F] CreateProcessW() failed, GetLastError() = %d\n", GetLastError());
        return false;
    }

    SIZE_T dllPathSize = (wcslen(corePath) + 1) * sizeof(WCHAR);
    LPVOID dllPath = NULL;
    HANDLE hTNew = NULL;
    if ((dllPath = VirtualAllocEx(pi.hProcess, NULL, dllPathSize, MEM_COMMIT, PAGE_READWRITE))
        && WriteProcessMemory(pi.hProcess, dllPath, corePath, dllPathSize, &dllPathSize)
        && (hTNew = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, dllPath, 0, NULL)))
    {
        // NOTE: Resume main thread only after LoadLibrary() finishes
        // This is for those shady programs that hooks LoadLibrary() on startup
        WaitForSingleObject(hTNew, INFINITE);
        CloseHandle(hTNew);
        ResumeThread(pi.hThread);
    }
    else
    {
        fwprintf(stderr, L"[E] Remote thread injection failed, GetLastError() = %d\n", GetLastError());
    }
    return true;
}

#endif

int main()
{
    _putws(L"HueShift Injector v0.0.3 (" __DATE__ ", Build tag \"" HUESHIFT_BUILD_TAG "\")\n");

    if (!SetPrivilege(GetCurrentProcess(), SE_DEBUG_NAME, TRUE))
    {
        fwprintf(stderr, L"[F] SetPrivilege(SeDebugPrivilege) failed, GetLastError() = %d\n", GetLastError());
        return 1;
    }

    WCHAR* _pFilePart;
    if (!SearchPathW(NULL, L"" HUESHIFT_CORE, NULL, MAX_PATH, corePath, &_pFilePart))
    {
        fwprintf(stderr, L"[F] SearchPath(" HUESHIFT_CORE ") failed, GetLastError() = %d\n", GetLastError());
        return 1;
    }
    wprintf(L"[I] " HUESHIFT_CORE " found at \"%s\"\n", corePath);

    if (!SearchPathW(NULL, L"" HUESHIFT_VICTIM, NULL, MAX_PATH, victimPath, &_pFilePart))
    {
        fwprintf(stderr, L"[F] SearchPath(" HUESHIFT_VICTIM ") failed, GetLastError() = %d\n", GetLastError());
        return 1;
    }
    wprintf(L"[I] " HUESHIFT_VICTIM " found at \"%s\"\n", victimPath);

    // NOTE: No miscalculations happened here; PathQuoteSpacesW() limits the buffer size to a maximum of MAX_PATH
    WCHAR* victimArgsBegin = PathGetArgsW(GetCommandLineW());
    size_t victimArgsSize = _countof(victimPath) + 1 + wcslen(victimArgsBegin) + 1;

    victimArgs = (WCHAR*)malloc(victimArgsSize * sizeof(WCHAR));
    if (!victimArgs)
    {
        fwprintf(stderr, L"[F] malloc() for victimArgs failed; What did you do?\n");
        return 42;
    }

    wcscpy_s(victimArgs, victimArgsSize, victimPath);
    PathQuoteSpacesW(victimArgs);
    wcscat_s(victimArgs, victimArgsSize, L" ");
    wcscat_s(victimArgs, victimArgsSize, victimArgsBegin);

    DWORD dwCreationFlags = CREATE_UNICODE_ENVIRONMENT;
#ifndef HUESHIFT_SHARE_CONSOLE
    dwCreationFlags |= CREATE_NEW_CONSOLE;
#endif

    PROCESS_INFORMATION pi = { 0 };
    if (LaunchVictim(dwCreationFlags, pi))
    {
        wprintf(L"[I] Victim launched (PID = %u), have fun.\n", pi.dwProcessId);
    }

    free(victimArgs);

    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }
    if (pi.hProcess)
    {
#ifdef HUESHIFT_SHARE_CONSOLE
        WaitForSingleObject(pi.hProcess, INFINITE);
#endif
        CloseHandle(pi.hProcess);
    }

    return 0;
}


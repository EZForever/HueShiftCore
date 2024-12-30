#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <delayimp.h>

#include <cstring>

#include "config.h"

#ifdef HUESHIFT_DUMMY_EXPORT
#   error "HUESHIFT_DUMMY_EXPORT interferes with the EAT; do not use with this module"
#endif

FARPROC WINAPI DllDelayHook(unsigned dliNotify, PDelayLoadInfo pdli)
{
    switch (dliNotify)
    {
    case dliNotePreLoadLibrary:
    {
        if (!_stricmp(pdli->szDll, HUESHIFT_HIJACK_FROM))
        {
            // HACK: Current impl. constructs pdli on stack, thus writable
            // This is not mentioned in the document, but should be intentional
            pdli->szDll = HUESHIFT_HIJACK_TO;
            return NULL;
        }
        break;
    }
    case dliStartProcessing:
    case dliNotePreGetProcAddress:
    case dliFailLoadLib:
    case dliFailGetProc:
    case dliNoteEndProcessing:
        break;
    }
    return NULL;
}

ExternC const PfnDliHook __pfnDliNotifyHook2 = DllDelayHook;
ExternC const PfnDliHook __pfnDliFailureHook2 = DllDelayHook;


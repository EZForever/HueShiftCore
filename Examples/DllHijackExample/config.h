#pragma once

#define HUESHIFT_BUILD_TAG "DllHijackExample"

#define HUESHIFT_ALLOC_CONSOLE

// This example hijacks VERSION.DLL, a common target

// Important: Set this to the LIBRARY directive name used in DEF file
#define HUESHIFT_HIJACK_FROM "VERSION.DLL"

// This path is in NT namespace to prevent Windows loader from parsing the path and skip loading the original DLL
// If you're not hijacking a Windows DLL, you can just rename the original DLL and use the new name of it here
#define HUESHIFT_HIJACK_TO "\\\\.\\GLOBALROOT\\SystemRoot\\System32\\VERSION.DLL"


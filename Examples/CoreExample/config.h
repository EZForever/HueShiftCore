#pragma once
// HueShiftCore CoreCommon sample config file

// (Mandatory) Build tag to identify a certain HueShiftCore Core+Injector combo
#define HUESHIFT_BUILD_TAG "BasicExample"

// Attach to or allocate a console window on load and enable debug logging (see logging.h)
// Useful for GUI victim programs
#define HUESHIFT_ALLOC_CONSOLE

// Add a dummy export entry to the DLL
// Detours sometimes fail to inject a DLL if it contains no exports
//#define HUESHIFT_DUMMY_EXPORT

// Source & destination paths for DLL hijacking
// See DllHijackExample for details
//#define HUESHIFT_HIJACK_FROM ""
//#define HUESHIFT_HIJACK_TO ""


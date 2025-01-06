#pragma once
// HueShiftCore InjectorCommon sample config file

// (Mandatory) Build tag to identify a certain HueShiftCore Core+Injector combo
#define HUESHIFT_BUILD_TAG "BasicExample"

// (Mandatory) Name of Core DLL to inject; should be placed in the same directory as the Injector
#define HUESHIFT_CORE "CoreExample.dll"

// (Mandatory) Victim program name to be injected; full path will be resolved at run-time by searching PATH
#define HUESHIFT_VICTIM "cmd.exe"

// Allow the victim program to share the console Injector is in, and wait for its completion
// Useful for CLI victim programs
#define HUESHIFT_SHARE_CONSOLE

// Use DetourCreateProcessWithDllEx() instead of custom routine for injecting Core DLL
// Prone to failure and breaks IatCore, not recommended
//#define HUESHIFT_LAUNCH_WITH_DETOURS


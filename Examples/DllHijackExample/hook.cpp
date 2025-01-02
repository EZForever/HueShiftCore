#include "hook.h"
#include "logging.h"

// You can still define your hook instances like normal

HOOK_INSTANCE(GetSystemTime)
{
	VOID WINAPI HandlerRoutine(LPSYSTEMTIME lpSystemTime)
	{
		OriginalRoutine(lpSystemTime);

		LOG(L"[I] GetSystemTime(): Current UTC time is %02u:%02u\n", lpSystemTime->wHour, lpSystemTime->wMinute);
	}
}

// But for functions exported by the hijacked DLL, you MUST NOT use HOOK_INSTANCE_* macros
// Instead, define a redirection in exports.def (as shown there), and define your hook routine under the redirected name
// Pay attention to function prototype and calling convention as usual, as well as name mangling (use `extern "C"` whenever possible)

extern "C"
BOOL APIENTRY xGetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
	LOG(L"[I] GetFileVersionInfoW(): Called on file \"%s\", dwHandle = 0x%08X\n", lptstrFilename, dwHandle);

	// You can use the original routine directly; delay-load hook got you covered
	return GetFileVersionInfoW(lptstrFilename, 0, dwLen, lpData);
}


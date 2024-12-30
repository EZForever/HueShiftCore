#include "hook.h"
#include "logging.h"

HOOK_INSTANCE(GetSystemTime)
{
	VOID WINAPI HandlerRoutine(LPSYSTEMTIME lpSystemTime)
	{
		OriginalRoutine(lpSystemTime);

		LOG(L"[I] GetSystemTime(): Current UTC time is %02u:%02u\n", lpSystemTime->wHour, lpSystemTime->wMinute);
	}
}


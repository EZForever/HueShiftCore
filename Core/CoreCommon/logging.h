#include "config.h"

#include <conio.h>

#include <cstdio>

#ifdef HUESHIFT_ALLOC_CONSOLE
#	define LOG(...) fwprintf(stderr, __VA_ARGS__)
#	define PAUSE() (std::ignore = _getwch())
#else
#	define LOG(...) __noop()
#	define PAUSE() __noop()
#endif


#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tuple>
#include <vector>

extern HMODULE g_hModule;
extern std::vector<std::tuple<const char*, PVOID*, const PVOID>> g_hookInstances;

bool __HookInitialize(const char* name, PVOID* pOriginal, const PVOID handler);


#pragma once
#include <windows.h>

HMODULE WINAPI myGetModuleHandle(LPCWSTR sModuleName);
FARPROC WINAPI myGetProcAddress(HMODULE hMod, const char* sProcName);

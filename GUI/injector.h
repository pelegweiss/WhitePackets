#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>


DWORD GetProcId(const wchar_t* procName);
bool inject(const wchar_t* procName, const wchar_t* dllPath);
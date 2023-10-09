#pragma once
#include <Windows.h>
class Hook
{
public:
	bool bStatus{ false };
	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	uintptr_t len{ 0 };
	BYTE originalBytes[10]{ 0 };

	Hook(BYTE* src, BYTE* dst, uintptr_t len);
	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
	void Enable();
	void Disable();
	void Toggle();
};
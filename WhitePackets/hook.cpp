#include "hook.h"
#include <iostream>
#include "strings.h"
#include "myAPI.h"
typedef BOOL (WINAPI* dVProtect)(LPVOID, SIZE_T, DWORD, PDWORD);

Hook::Hook(BYTE* src, BYTE* dst, uintptr_t len)
{
    this->src = src;
    this->dst = dst;
    this->len = len;
}
void Hook::Enable()
{
	memcpy(originalBytes, src, len);
	Detour32(src, dst, len);
	this->bStatus = true;
}
void Hook::Disable()
{
	memcpy(src, originalBytes, len);
	this->bStatus = false;
}
void Hook::Toggle()
{
	if (this->bStatus)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

bool Hook::Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
    DecryptXOR((char*)vAllocEncrypted, vAllocEncryptedLen, eKey, eKeyLen);
    dVProtect dVProtectPtr = (dVProtect)myGetProcAddress(myGetModuleHandle(L"Kernel32.dll"), "VirtualAlloc");
    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    dVProtectPtr(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
    uintptr_t relatveAddress = dst - src - 5;
    *src = 0xE9;
    *(uintptr_t*)(src + 1) = relatveAddress;
    int offset = (int)len - 5;
    for (int i = 0; i < offset; i++)
    {
        *(src + 5 + i) = 0x90;
    }
    dVProtectPtr(src, len, curProtection, &curProtection);
    return true;
}
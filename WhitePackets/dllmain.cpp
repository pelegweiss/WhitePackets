#include "logic.h"
DWORD WINAPI HackThread(HMODULE hModule)
{
    // Create a console for the DLL.
    AllocConsole();
    FILE* p;
    freopen_s(&p, "CONOUT$", "w", stdout);
    freopen_s(&p, "CONIN$", "r", stdin);


    mainLogic();

    // Clean up and close the console.
    fclose(p);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
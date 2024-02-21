#include "logic.h"

extern "C" __declspec(dllexport) void RunYourCode(void) {
    AllocConsole();
    FILE* p;
    freopen_s(&p, "CONOUT$", "w", stdout);
    freopen_s(&p, "CONIN$", "r", stdin);


    mainLogic();

    // Clean up and close the console.
    fclose(p);
    FreeConsole();

}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) 
{

		switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
	}
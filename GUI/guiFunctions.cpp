#include "guiFunctions.h"

bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath, std::function<void()> pipeHandlerFunc, Pipe pipeToCreate)
{
    ShellExecute(nullptr, L"open", maplestoryPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    Sleep(3000);
    int i = 0;
    DWORD procID = GetProcId(L"HeavenMS-localhost-WINDOW.exe");
    while (procID == NULL && i < 10)
    {
        Sleep(250);
        i++;
        procID = GetProcId(L"HeavenMS-localhost-WINDOW.exe");

    }
    if (i == 10)
    {
        std::cout << "Failed to launch" << std::endl;
        return false;
    }
    else
    {
        std::cout << "Maplestory launched sucssesfully going to inject" << std::endl;
        Sleep(1000);
        if (inject(L"HeavenMS-localhost-WINDOW.exe", dllPath.c_str()) == true)
        {
            std::thread thread_obj(pipeHandlerFunc);
            pipeToCreate.createPipe();
            pipeToCreate.waitForClient();
            return true;
        }
        else
        {
            std::cout << "Failed injecting DLL" << std::endl;
            return false;

        }
    }
}

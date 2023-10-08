#include "injector.h"
DWORD GetProcId(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

bool inject(const wchar_t* procName, const wchar_t* dllPath)
{
    DWORD procId = 0;
    while (!procId)
    {
        procId = GetProcId(procName);
        Sleep(30);
    }
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {

        void* loc = VirtualAllocEx(hProc, 0, (wcslen(dllPath) + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        DWORD result = WriteProcessMemory(hProc, loc, dllPath, (wcslen(dllPath) + 1) * sizeof(wchar_t), NULL);
        if (result == NULL)
        {
            return false;
        }

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, loc, 0, 0);

        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    else
    {
        return false;
    }

    if (hProc)
    {
        CloseHandle(hProc);
    }
    return true;
}

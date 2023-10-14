#include "guiFunctions.h"
#include <sstream>
#include "controls.h"
#include <iomanip>

extern ListView* lvPackets;
Pipe pipeToGui(L"pipeToGui");

void messagesHandler(pipeMessage message)
{
    switch (message.id)
    {
        case 0:
        {

        }
        break;
        case 1:
        {
            if (!sniff)
                break;

            // Convert caller address and header to byte vectors
            std::vector<BYTE> callerAddressBytes = convertToBytes(message.data.callerAddress);
            std::vector<BYTE> headerBytes = convertToBytes(message.data.header);

            std::wstring callerAddressString = L"0x" + toHexString(callerAddressBytes);
            std::wstring headerString = toHexString(headerBytes);

            std::vector<std::wstring> buf;
            buf.emplace_back(callerAddressString);
            buf.emplace_back(L"Send");
            buf.emplace_back(headerString);

            message.data.segments.erase(message.data.segments.begin());
            std::wstring data;
            data = segmentsToWstring(message.data.segments);
            buf.emplace_back(data);
            lvPackets->add_item(buf);

        }
        break;
        case 2:
        {
            if (!sniff)
                break;

            // Convert caller address and header to byte vectors
            std::vector<BYTE> callerAddressBytes = convertToBytes(message.data.callerAddress);
            std::vector<BYTE> headerBytes = convertToBytes(message.data.header);

            std::wstring callerAddressString = L"0x" + toHexString(callerAddressBytes);
            std::wstring headerString = toHexString(headerBytes);

            std::vector<std::wstring> buf;
            buf.emplace_back(callerAddressString);
            buf.emplace_back(L"Recv");
            buf.emplace_back(headerString);

            message.data.segments.erase(message.data.segments.begin());
            std::wstring data;
            data = segmentsToWstring(message.data.segments);
            buf.emplace_back(data);
            lvPackets->add_item(buf);


            break;


        }
        break;

    }
}
void pipeHandler()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    while (!pipeToGui.connectPipe())
    {
        std::cout << "Failed to connect pipe, retrying" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    pipeMessage message = pipeToGui.readMessage();
    while (message.id != -1)
    {

        messagesHandler(message);
        message = pipeToGui.readMessage();
    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}
bool runMaplestory(std::wstring maplestoryPath, std::wstring dllPath)
{
    const wchar_t* mPath = L"C:\\Users\\Peleg\\Desktop\\Personal\\Heaven.MS.v83.pack\\HeavenMS - Source\\HeavenMS-master\\HeavenMS-master\\MapleStory\\HeavenMS-localhost-WINDOW.exe";
    const wchar_t* dPath = L"C:\\WhitePackets\\WhitePackets\\Debug\\WhitePackets.dll";
    ShellExecute(nullptr, L"open", mPath, nullptr, nullptr, SW_SHOWNORMAL);
    Sleep(3000);
    int i = 0;
    DWORD procID = GetProcId(L"HeavenMS-localhost-WINDOW.exe");
    while (procID == NULL && i < 10)
    {
        Sleep(50);
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
        if (inject(L"HeavenMS-localhost-WINDOW.exe", dPath) == true)
        {
            //std::thread thread_obj(pipeHandlerFunc);
            DWORD tID;
            HANDLE t1 = CreateThread(
                0,
               0,
                (LPTHREAD_START_ROUTINE)pipeHandler,
                0,
                0,
                &tID
            );

            //pipeToCreate.createPipe();
            //pipeToCreate.waitForClient();
            return true;
        }
        else
        {
            std::cout << "Failed injecting DLL" << std::endl;
            return false;

        }
    }
}
